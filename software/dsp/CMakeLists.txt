cmake_minimum_required(VERSION 3.25)

project(dsp LANGUAGES CXX)

# DSL은 현재 header-only 템플릿 라이브러리다. INTERFACE target을 두면 실행 파일과
# 테스트가 include 경로와 C++23 요구사항을 같은 방식으로 상속한다(명세 §1).
add_library(aris_dsp INTERFACE)
target_compile_features(aris_dsp INTERFACE cxx_std_23)
target_include_directories(
    aris_dsp
    INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# 기존 IQ sample 로딩 애플리케이션. DSL 구현과 별도 타깃으로 유지한다.
add_executable(dsp src/main.cpp)
target_link_libraries(dsp PRIVATE aris_dsp)

include(CTest)
