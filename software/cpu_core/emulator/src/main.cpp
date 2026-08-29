#include <filesystem>
#include <print>
#include "CPU.hpp"
#include "RAM.hpp"

#ifndef ARIS_DEFAULT_FIRMWARE_PATH
#define ARIS_DEFAULT_FIRMWARE_PATH "firmware.bin"
#endif

int main(int argc, char* argv[]) {
    constexpr aris::word ram_size = 32 * 1024;
    constexpr aris::word firmware_entry_point = 0x00002000;

    const std::filesystem::path firmware_path = argc > 1
        ? std::filesystem::path(argv[1])
        : std::filesystem::path(ARIS_DEFAULT_FIRMWARE_PATH);

    aris::RAM ram{ram_size};
    ram.load(firmware_path);

    aris::CPU cpu{ram};
    cpu.set_program_counter(firmware_entry_point);

    std::println("Loaded firmware: {}", firmware_path.string());
    std::println("Starting PC: 0x{:08x}", cpu.program_counter());

    while (cpu.step()) {
    }

    std::println("Stopped at PC: 0x{:08x}", cpu.program_counter());
    return 0;
}
