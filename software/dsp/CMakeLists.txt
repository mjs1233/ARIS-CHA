cmake_minimum_required(VERSION 3.25)

project(dsp LANGUAGES CXX)
add_executable(dsp src/main.cpp)

target_compile_features(dsp PRIVATE cxx_std_23)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
