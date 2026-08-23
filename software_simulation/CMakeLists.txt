cmake_minimum_required(VERSION 4.2)
project(ARIS_RV32I)

set(CMAKE_CXX_STANDARD 23)

add_executable(ARIS_RV32I
        src/main.cpp
        src/CPU.cpp
        src/Config.cpp
        src/Decoder.cpp
        src/ExecutionStage.cpp
        src/RAM.cpp
        src/RegisterFile.cpp
)

include_directories(include)

set(ARIS_DEFAULT_FIRMWARE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../firmware/cmake-build-debug/firmware.bin" CACHE FILEPATH
    "Firmware binary loaded by the simulator when no command-line path is provided")
target_compile_definitions(ARIS_RV32I PRIVATE ARIS_DEFAULT_FIRMWARE_PATH="${ARIS_DEFAULT_FIRMWARE_PATH}")

target_link_libraries(ARIS_RV32I PRIVATE stdc++exp)