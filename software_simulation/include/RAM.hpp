//
// Created by tgian on 2026-08-19.
//

#ifndef ARIS_RV32I_RAM_HPP
#define ARIS_RV32I_RAM_HPP
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <vector>
#include "Global.hpp"

namespace aris {

    class RAM {
    public:
        RAM(size_t size) : data(size) {}

        void load(const std::filesystem::path& path) {
            std::ifstream file(path, std::ios::binary | std::ios::ate);
            if (!file) {
                throw std::runtime_error("failed to open binary file: " + path.string());
            }

            const auto end = file.tellg();
            if (end < 0) {
                throw std::runtime_error("failed to determine binary file size: " + path.string());
            }

            const auto file_size = static_cast<size_t>(end);
            if (file_size > data.size()) {
                throw std::out_of_range("binary file is larger than RAM: " + path.string());
            }

            std::fill(data.begin(), data.end(), 0);
            file.seekg(0);
            file.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(file_size));
            if (!file && !file.eof()) {
                throw std::runtime_error("failed to load binary file: " + path.string());
            }
        }

        void write(word value,word address) {
            data[address + 0] = value;
            data[address + 1] = value << 8;
            data[address + 2] = value << 16;
            data[address + 3] = value << 24;
        }

        word read(word address) {
            word value = 0;
            value += data[address + 0];
            value += data[address + 1] << 8;
            value += data[address + 2] << 16;
            value += data[address + 3] << 24;
            return value;
        }

    private:
        std::vector<aris::byte> data;
    };
}
#endif //ARIS_RV32I_RAM_HPP
