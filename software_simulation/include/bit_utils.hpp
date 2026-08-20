//
// Created by takakura on 8/20/26.
//

#ifndef ARIS_RV32I_BIT_UTILS_HPP
#define ARIS_RV32I_BIT_UTILS_HPP
#include <cstdint>
#include <limits>

namespace aris::utils {

    template <std::uint8_t Start, std::uint8_t End>
    [[nodiscard]]
    constexpr std::uint32_t take_bits(std::uint32_t value) noexcept {
        static_assert(Start <= End);
        static_assert(End < 32);

        constexpr std::uint8_t width = End - Start + 1;

        constexpr std::uint32_t mask =
            std::numeric_limits<std::uint32_t>::max() >> (32 - width);

        return (value >> Start) & mask;
    }

    inline auto take_rd = [](uint32_t value) noexcept -> std::uint32_t {
        return take_bits<7,11>(value);
    };

    inline auto take_funct3 = [](uint32_t value) noexcept -> std::uint32_t {
        return take_bits<12,14>(value);
    };

    inline auto take_rs1 = [](uint32_t value) noexcept -> std::uint32_t {
        return take_bits<15,19>(value);
    };

    inline auto take_rs2 = [](uint32_t value) noexcept -> std::uint32_t {
        return take_bits<20,24>(value);
    };


}
#endif //ARIS_RV32I_BIT_UTILS_HPP
