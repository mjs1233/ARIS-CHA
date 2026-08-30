# DSP Stream DSL 구현 안내서

이 문서는 `DSP Stream DSL — C++23 Implementation Specification`을 실제 C++ 타입과
템플릿으로 옮긴 현재 골격을 설명한다. 코드를 처음 읽는 사람은 아래 순서대로 보는 것이
가장 쉽다.

각 템플릿 표현식과 `std::move`, `std::forward`, fold expression이 실제로 어떤 타입과
값을 조작하는지 한 줄씩 따라가려면
[`stream_dsl_code_walkthrough.md`](stream_dsl_code_walkthrough.md)를 먼저 읽어도 된다.

1. `stream.hpp`: 사용자가 보는 파이프라인 문법
2. `function_traits.hpp`: callable 시그니처를 타입 정보로 변환
3. `pipeline_analysis.hpp`: 의미 태그 의존성과 논리 버전을 분석
4. `stream_context.hpp`: 런타임 값을 태그별 슬롯에 저장
5. `executable_stream.hpp`: 분석 결과를 사용해 스테이지를 순서대로 실행
6. `preserve_tag.hpp`: 표현 수준 알고리즘을 의미 태그 세계에 연결

## 1. 명세의 세 가지 정체성

명세 §3, §26, §43은 다음 세 개를 절대로 혼동하지 말라고 요구한다.

| 구분 | 코드 예 | 현재 구현에서 쓰이는 곳 |
|---|---|---|
| 표현 정체성 | `float` | `preserve_tag` 내부 알고리즘 |
| 의미 정체성 | `linear_gain` | 의존성 탐색과 context 슬롯 |
| 논리 값 정체성 | `linear_gain#1` | `value_version<linear_gain, 1>` |

`linear_gain`과 `db_gain`이 모두 `float` 하나를 담더라도 서로 다른 의미 태그다.
따라서 context 조회와 의존성 분석은 `float`가 아니라 태그 전체 타입을 키로 사용한다.

## 2. 공개 API에서 시작되는 흐름

현재 골격의 사용법은 다음과 같다.

```cpp
auto pipeline =
    aris::dsp::stream{}
    | estimate_gain{}
    | aris::dsp::preserve_tag<linear_gain>(filter{})
    | apply_gain{};

auto executable =
    std::move(pipeline).assemble<sample>();

auto context = executable.make_context(sample{1.0F});
executable.exec(context);
```

각 호출의 의미는 다음과 같다.

- `operator|`: callable 객체를 튜플 뒤에 보관한다. 실행하지 않는다(명세 §12).
- `assemble<sample>()`: `sample`을 초기 의미 태그로 놓고 전체 파이프라인을
  컴파일타임에 분석한다(명세 §14, §15).
- `make_context(...)`: 분석으로 구한 전체 의미 태그 집합을 위한 런타임 슬롯을 만들고,
  초기 태그 값만 유효 상태로 채운다(명세 §16~§19).
- `exec(...)`: 각 callable의 인자 태그를 context에서 꺼내고 반환 태그를 다시 저장한다
  (명세 §20, §35).

## 3. 왜 `assemble<InitialTags...>()`인가

명세에는 구현 전에 결정해야 하는 시점 문제가 있다.

- §14: `assemble()`이 의존성 검증과 CREATE/UPDATE 분류를 수행해야 한다.
- §15: 초기 값 집합은 나중에 `exec(block)`에 전달되는 block이 결정한다.

그러나 초기 태그를 모르면 첫 스테이지의 의존성도 CREATE/UPDATE도 판정할 수 없다.
현재 골격은 이 문제를 숨기지 않고 초기 태그를 조립 시점의 템플릿 인자로 받는다.

```cpp
pipeline.assemble<sample, configuration>();
```

이 선택은 다음을 가능하게 한다.

1. 조립 시점에 모든 의존성을 검증한다.
2. 파이프라인 도중 생성되는 태그까지 모아 `context_type`을 확정한다.
3. `make_context()`가 초기값을 정확히 한 번씩 받았는지 제약식으로 확인한다.

이 API는 현재 골격에서 의도적으로 선택한 구현 방향이며, 최종 공개 계약을 확정할 때
명세 본문의 `assemble()` 표기와 함께 다시 결정할 수 있다.

## 4. 컴파일타임 분석의 실제 모양

예를 들어 다음 상태를 생각한다.

```text
현재 상태:
    sample#0
    linear_gain#1

다음 스테이지:
    (sample, linear_gain) -> sample
```

`pipeline_analysis`는 이를 아래 타입에 대응시킨다.

```cpp
analysis_state<
    current_value<sample, 0>,
    current_value<linear_gain, 1>
>
```

스테이지 분석 과정은 다음 순서다.

1. `stage_signature`가 입력 태그 목록과 출력 태그를 추출한다.
2. 모든 입력 태그가 현재 상태에 있는지 검사한다(명세 §29).
3. 출력 `sample`이 이미 존재하는지 검사한다.
4. 이미 존재하며 입력에도 `sample`이 있으므로 UPDATE로 판정한다(명세 §24 Rule B).
5. 입력은 `sample#0`, `linear_gain#1`로 기록한다.
6. 출력은 `sample#1`로 기록한다(명세 §27).
7. 다음 상태의 현재 `sample` 버전을 1로 교체한다.

스테이지 하나의 분석 결과는 대략 다음 타입이다.

```cpp
stage_metadata<
    3,
    apply_gain,
    meta::type_list<
        value_version<sample, 0>,
        value_version<linear_gain, 1>
    >,
    value_version<sample, 1>,
    write_kind::update
>
```

이 타입에는 런타임 데이터가 없다. 추후 liveness를 구현할 때 birth/last-consumer
정보를 붙일 수 있는 컴파일타임 IR의 뼈대다(명세 §31~§33).

## 5. CREATE, UPDATE, 오류 판정

명세 §22~§24와 §44를 그대로 적용한다.

```text
출력 태그가 현재 없음
    -> CREATE, 버전 0 생성

출력 태그가 현재 있고 입력에도 있음
    -> UPDATE, 현재 버전 + 1 생성

출력 태그가 현재 있지만 입력에는 없음
    -> ambiguous overwrite 컴파일 오류
```

세 번째 규칙이 중요한 이유는, 다른 값으로부터 우연히 같은 태그를 반환한 스테이지가
기존 의미 상태를 소리 없이 덮어쓰는 것을 막기 위해서다.

## 6. 런타임 context

명세 §17에 따라 물리 저장소 최적화를 하지 않고 태그마다 독립 슬롯을 둔다.

```cpp
std::tuple<
    semantic_slot<sample>,
    semantic_slot<linear_gain>,
    semantic_slot<db_gain>
>
```

각 슬롯은 `std::optional<Tag>`다. 이 구조 덕분에 다음이 가능하다.

- 태그가 기본 생성 불가능해도 context 자체는 생성할 수 있다.
- 태그가 move-only여도 `set<Tag>()`에서 이동 생성할 수 있다.
- 파이프라인 도중 아직 생성되지 않은 태그는 invalid 상태로 표현한다.
- 의미 태그 타입 자체를 `std::get<semantic_slot<Tag>>`의 키로 사용한다.

논리 버전은 분석 IR에만 존재한다. 런타임에서는 UPDATE 시 동일 태그 슬롯의 객체를
새 값으로 교체한다(명세 §25).

## 7. 실행 단계

`executable_stream::exec()`는 인덱스 0부터 끝까지 comma fold로 실행한다.
각 스테이지마다 다음 동작을 수행한다.

```text
stage_signature에서 input_tags 조회
    -> context.get<Tag>()... 구성
    -> std::invoke(stage, ...)
    -> 반환 객체를 context.set<OutputTag>()에 저장
```

사용자 스테이지는 context나 스테이지 인덱스를 전혀 알지 못한다(명세 §4, §20).
스테이지 예외는 잡아 변환하지 않으므로 `exec()` 호출자에게 그대로 전파된다(명세 §36).

## 8. `preserve_tag`

명세 §9~§11의 흐름을 직접 구현한다.

```text
Tag
    -> input.value
    -> F(value_type)
    -> Tag{result}
```

이 어댑터는 의미 타입을 바꾸지 않는다. 예를 들어 `linear_gain`을 `db_gain`으로
바꾸려면 반드시 별도의 명시적 변환 스테이지를 작성해야 한다.

## 9. 현재 구현 범위와 다음 작업

현재 골격에 들어 있는 것:

- 단일 비템플릿 `operator()` 분석
- `const`/비-`const`, `noexcept` 조합 분석
- 읽기 전용 입력 제약
- 의존성 검증
- CREATE/UPDATE와 논리 버전
- 태그별 독립 슬롯
- 동기 순차 실행
- `preserve_tag`
- move-only 태그와 예외 전파 테스트

다음 우선순위:

1. 각 실패 규칙을 별도 translation unit으로 만드는 compile-fail 테스트
2. 진단 템플릿 구조를 다듬어 부가 오류 메시지 줄이기
3. 태그 형식과 `preserve_tag` 형식을 concepts로 명시
4. 필요할 경우 모든 실제 호출을 바탕으로 조건부 `noexcept` 추가

명세 §31~§33의 liveness, 자동 invalidation, 물리 저장소 재사용은 현재 구현하지 않는다.
이를 서두르면 v1 의미론 검증과 저장소 최적화가 섞이므로 IR이 안정된 뒤 진행한다.
