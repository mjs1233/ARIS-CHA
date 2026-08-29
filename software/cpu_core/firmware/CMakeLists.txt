cmake_minimum_required(VERSION 3.22)

# This is a dedicated bare-metal RV32I project. Select its cross compiler before
# project() so IDE CMake profiles cannot configure it with the host compiler.
set(RISCV_TOOLCHAIN_BIN "/home/takakura/opt/riscv32im/bin" CACHE PATH
    "Directory containing RISC-V GNU toolchain executables")
set(RISCV_TOOLCHAIN_PREFIX "riscv32-unknown-elf" CACHE STRING
    "Prefix of the RISC-V GNU toolchain executables")

find_program(RISCV_GCC
    NAMES ${RISCV_TOOLCHAIN_PREFIX}-gcc
    HINTS ${RISCV_TOOLCHAIN_BIN}
    REQUIRED
)
find_program(RISCV_GXX
    NAMES ${RISCV_TOOLCHAIN_PREFIX}-g++
    HINTS ${RISCV_TOOLCHAIN_BIN}
    REQUIRED
)
find_program(RISCV_OBJCOPY
    NAMES ${RISCV_TOOLCHAIN_PREFIX}-objcopy
    HINTS ${RISCV_TOOLCHAIN_BIN}
    REQUIRED
)
find_program(RISCV_OBJDUMP
    NAMES ${RISCV_TOOLCHAIN_PREFIX}-objdump
    HINTS ${RISCV_TOOLCHAIN_BIN}
    REQUIRED
)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv32)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_C_COMPILER ${RISCV_GCC} CACHE FILEPATH "RV32I C compiler" FORCE)
set(CMAKE_CXX_COMPILER ${RISCV_GXX} CACHE FILEPATH "RV32I C++ compiler" FORCE)
set(CMAKE_ASM_COMPILER ${RISCV_GCC} CACHE FILEPATH "RV32I assembler" FORCE)
set(CMAKE_OBJCOPY ${RISCV_OBJCOPY} CACHE FILEPATH "RISC-V GNU objcopy executable" FORCE)
set(CMAKE_OBJDUMP ${RISCV_OBJDUMP} CACHE FILEPATH "RISC-V GNU objdump executable" FORCE)

project(ARIS_RV32I_Firmware LANGUAGES C CXX ASM)

set(FIRMWARE_RAM_ORIGIN "0x00000000" CACHE STRING "RAM base address")
set(FIRMWARE_RAM_LENGTH "32K" CACHE STRING "Simulation RAM size for the fixed memory map" FORCE)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld.in
    ${CMAKE_CURRENT_BINARY_DIR}/linker.ld
    @ONLY
)

add_executable(firmware
    src/startup.S
    src/main.cpp
)

target_compile_options(firmware PRIVATE
    -march=rv32i
    -mabi=ilp32
    -ffreestanding
    -fno-exceptions
    -fno-rtti
    -ffunction-sections
    -fdata-sections
    -Wall
    -Wextra
    -Werror
)

target_link_options(firmware PRIVATE
    -march=rv32i
    -mabi=ilp32
    -nostdlib
    -nostartfiles
    -Wl,--gc-sections
    -Wl,--build-id=none
    -Wl,-T,${CMAKE_CURRENT_BINARY_DIR}/linker.ld
)

set(FIRMWARE_BIN ${CMAKE_CURRENT_BINARY_DIR}/firmware.bin)
set(FIRMWARE_ASM ${CMAKE_CURRENT_BINARY_DIR}/firmware.asm)
add_custom_command(TARGET firmware POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:firmware> ${FIRMWARE_BIN}
    COMMAND ${CMAKE_COMMAND}
        -DOBJDUMP=${CMAKE_OBJDUMP}
        -DINPUT=$<TARGET_FILE:firmware>
        -DOUTPUT=${FIRMWARE_ASM}
        -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/write_disassembly.cmake
    BYPRODUCTS ${FIRMWARE_BIN} ${FIRMWARE_ASM}
    COMMENT "Generating firmware binary and assembly listing"
)
