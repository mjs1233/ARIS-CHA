cmake_minimum_required(VERSION 4.3)
project(ARIS_RV32I)

set(CMAKE_CXX_STANDARD 23)

add_executable(ARIS_RV32I
        src/main.cpp

)

include_directories(include)

target_link_libraries(ARIS_RV32I PRIVATE stdc++exp)