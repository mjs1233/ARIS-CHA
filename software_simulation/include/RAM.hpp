//
// Created by tgian on 2026-08-19.
//

#ifndef ARIS_RV32I_RAM_HPP
#define ARIS_RV32I_RAM_HPP
#include <cstddef>
#include <filesystem>
#include <vector>
#include "Global.hpp"

namespace aris {

    class RAM {
    public:
        explicit RAM(std::size_t size);

        void load(const std::filesystem::path& path);
        void write(word value, word address);
        [[nodiscard]] word read(word address) const;

    private:
        std::vector<byte> data;
    };
}
#endif // ARIS_RV32I_RAM_HPP
