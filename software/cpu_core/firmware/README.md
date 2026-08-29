# ARIS RV32I firmware

This project builds a freestanding RV32I ELF and flat binary for the simulator.

## Prerequisite

Install a RISC-V GNU toolchain that provides riscv32-unknown-elf-gcc and
riscv32-unknown-elf-objcopy. The default toolchain directory is
/home/takakura/opt/riscv32im/bin; override RISCV_TOOLCHAIN_BIN when needed.

The project selects this compiler itself before CMake calls project(), so IDE
profiles cannot accidentally use the host C++ compiler.

## Build

cmake -S firmware -B firmware/cmake-build-debug -G Ninja
cmake --build firmware/cmake-build-debug

The outputs are firmware/cmake-build-debug/firmware (ELF) and
firmware/cmake-build-debug/firmware.bin (flat binary), and
firmware/cmake-build-debug/firmware.asm (final disassembly).

## Memory map

The linker script maps one RAM region at 0x00000000 with size 64K. This matches
RAM::load(), which preloads the binary at RAM address zero. The current simulator
constructs 32 KiB of RAM, so this linker map intentionally fixes the RAM size to 32 KiB.
