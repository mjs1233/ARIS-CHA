#ifndef ARIS_DSP_META_TYPE_LIST_HPP
#define ARIS_DSP_META_TYPE_LIST_HPP

/*
 * 컴파일타임 타입 목록 유틸리티
 * ==============================
 *
 * 명세 대응:
 *   - §14 assemble()의 의존성/태그 집합 분석
 *   - §22~§24 CREATE/UPDATE 판정
 *   - §26~§27 논리 값 버전 관리
 *   - §43 의미 정체성과 논리 값 정체성의 분리
 *
 * 이 파일의 type_list는 런타임 컨테이너가 아니다. 객체도 만들지 않고 메모리도
 * 차지하지 않는다. `type_list<A, B>`라는 "타입 하나" 안에 A와 B라는 타입 목록을
 * 인코딩해서 템플릿 계산의 입력/출력으로 사용한다.
 *
 * 예를 들어 현재 사용 가능한 의미 태그 집합은 다음처럼 표현된다.
 *
 *     meta::type_list<sample, linear_gain, db_gain>
 *
 * 이 작은 유틸리티 계층을 별도로 둔 이유는 파이프라인 의미 분석 코드가
 * std::tuple의 런타임 저장 의미에 의존하지 않게 하기 위해서다.
 */

#include <concepts>
#include <cstddef>
#include <type_traits>

namespace aris::dsp::meta {

// 타입 0개 이상을 순서대로 보관하는 순수 타입 컨테이너다.
// 순서는 스테이지 인자 순서와 최초 태그 발견 순서를 보존할 때 사용한다.
template <typename... Ts>
struct type_list {};

// type_list의 원소 수를 컴파일타임 상수로 얻는다.
// executable_stream::make_context()가 초기 태그 개수와 실제 초기값 개수가
// 같은지 검사할 때 사용한다(명세 §15, §18).
template <typename List>
struct size;

template <typename... Ts>
struct size<type_list<Ts...>> : std::integral_constant<std::size_t, sizeof...(Ts)> {};

template <typename List>
inline constexpr std::size_t size_v = size<List>::value;

// I번째 타입을 꺼낸다. 런타임 인덱싱이 아니라 템플릿 재귀다.
// 테스트에서 stage_metadata의 분석 순서를 확인하는 용도로도 사용한다.
template <typename List, std::size_t I>
struct at;

// 재귀 종료점: 요청한 인덱스가 0이면 현재 Head가 답이다.
template <typename Head, typename... Tail>
struct at<type_list<Head, Tail...>, 0> {
    using type = Head;
};

// 아직 0이 아니면 Head를 버리고 I-1번째 타입을 계속 찾는다.
template <typename Head, typename... Tail, std::size_t I>
struct at<type_list<Head, Tail...>, I> : at<type_list<Tail...>, I - 1> {};

template <typename List, std::size_t I>
using at_t = typename at<List, I>::type;

// 의미 태그 T가 List 안에 존재하는지 검사한다.
// 명세 §29의 "현재 이 의미 값이 사용 가능한가?"를 구현하는 가장 기본적인 연산이다.
template <typename List, typename T>
struct contains;

template <typename... Ts, typename T>
struct contains<type_list<Ts...>, T>
    : std::bool_constant<(std::same_as<Ts, T> || ...)> {};

template <typename List, typename T>
inline constexpr bool contains_v = contains<List, T>::value;

// 같은 의미 태그가 목록에 두 번 들어가는 것을 막는다.
// context는 std::get<semantic_slot<Tag>>로 슬롯을 찾기 때문에 Tag가 중복되면
// 타입 기반 조회 자체가 모호해진다. 명세 §2의 "태그가 의미를 유일하게 식별한다"는
// 전제를 컨테이너 수준에서도 보장한다.
template <typename List>
struct all_unique;

// 빈 목록에는 중복이 없다.
template <>
struct all_unique<type_list<>> : std::true_type {};

// Head가 Tail에 없고 Tail 자체도 모두 유일해야 전체 목록이 유일하다.
template <typename Head, typename... Tail>
struct all_unique<type_list<Head, Tail...>>
    : std::bool_constant<
          !contains_v<type_list<Tail...>, Head>
          && all_unique<type_list<Tail...>>::value
      > {};

template <typename List>
inline constexpr bool all_unique_v = all_unique<List>::value;

// 새 타입을 목록 끝에 무조건 추가한다.
// 파이프라인 스테이지 순서 및 stage_metadata 순서를 유지하기 위해 뒤에 붙인다.
template <typename List, typename T>
struct push_back;

template <typename... Ts, typename T>
struct push_back<type_list<Ts...>, T> {
    using type = type_list<Ts..., T>;
};

template <typename List, typename T>
using push_back_t = typename push_back<List, T>::type;

// 이미 존재하는 의미 태그라면 그대로 두고, 처음 등장한 태그만 뒤에 추가한다.
// 현재 분석기는 analysis_state에서 직접 같은 동작을 수행하지만, 이후 IR 확장 시
// 태그 집합을 조합할 때 사용할 수 있도록 기본 연산으로 둔다.
template <typename List, typename T>
using push_back_unique_t = std::conditional_t<
    contains_v<List, T>,
    List,
    push_back_t<List, T>
>;

// type_list<Ts...>의 Ts...를 다른 variadic 클래스 템플릿에 그대로 옮긴다.
//
// 예:
//   type_list<A, B, C>
//       -> rebind<..., stream_context>
//       -> stream_context<A, B, C>
//
// 이 변환이 컴파일타임 분석 결과(all_tags)를 런타임 저장 타입으로 연결한다.
template <typename List, template <typename...> typename Target>
struct rebind;

template <typename... Ts, template <typename...> typename Target>
struct rebind<type_list<Ts...>, Target> {
    using type = Target<Ts...>;
};

template <typename List, template <typename...> typename Target>
using rebind_t = typename rebind<List, Target>::type;

} // namespace aris::dsp::meta

#endif
