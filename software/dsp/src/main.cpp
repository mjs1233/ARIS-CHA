#include <iostream>
#include <complex>
#include <cstdint>
#include <vector>
#include <fstream>
#include <filesystem>
#include <print>
#include "iq_signal.hpp"
#include "stream/stream_pool.hpp"

int main(int argc, char* argv[]) {

    /*
    if (argc != 2) {
        std::print("[fatal] enter input file\n");
        return 0;
    }

    uint64_t size = std::filesystem::file_size(argv[1]);
    std::vector<IQ_Signal<int8_t>> signals;
    signals.reserve(size);

    std::filesystem::path input_file{argv[1]};
    std::ifstream file{input_file, std::ios::in | std::ios::binary};

    constexpr size_t buffer_size = 4096;
    uint8_t buffer[buffer_size] = {};

    while (!file.eof()) {
        file.read(reinterpret_cast<char*>(buffer), buffer_size);

        for (int i = 0; i < buffer_size; i+=2) {
            auto I = static_cast<int8_t>(static_cast<int16_t>(buffer[i + 0]) - 128);
            auto Q = static_cast<int8_t>(static_cast<int16_t>(buffer[i + 1]) - 128);
            signals.emplace_back(I,Q);
        }

    }


*/

    return 0;
}
