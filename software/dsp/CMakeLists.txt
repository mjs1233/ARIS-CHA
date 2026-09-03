cmake_minimum_required(VERSION 3.25)

project(dsp LANGUAGES CXX)
add_executable(dsp)

add_subdirectory(src)

target_compile_features(dsp PRIVATE cxx_std_23)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_definitions(dsp PRIVATE
        ARIS_DSP_RESOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/res"
)
