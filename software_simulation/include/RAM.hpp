//
// Created by tgian on 2026-08-19.
//

#ifndef ARIS_RV32I_RAM_HPP
#define ARIS_RV32I_RAM_HPP
#include <vector>
#include <filesystem>
#include "Global.hpp"

namespace aris {

    class RAM {
    public:
        RAM(size_t size) : data(size) {}

        void load(std::filesystem::path p);

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
