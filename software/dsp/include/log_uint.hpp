//
// Created by takakura on 9/3/26.
//

#ifndef DSP_LOG_UINT_HPP
#define DSP_LOG_UINT_HPP
#include <bit>
#include <cstdint>
namespace aris::dsp {

    struct log_uint8 {
        constexpr log_uint8(std::uint16_t value) noexcept {
            if (value < 16) {
                data = static_cast<std::uint8_t>(value);
                return;
            }

            const auto msb = std::bit_width(value) - 1;

            const auto exponent = msb - 3;
            const auto mantissa =
                (value >> (msb - 4)) & 0b00001111;

            data = static_cast<std::uint8_t>(
                (exponent << 4) | mantissa
            );
        }

        [[nodiscard]]
        constexpr std::uint16_t decode() const noexcept {
            const auto exponent = data >> 4;
            const auto mantissa = data & 0b00001111;

            if (exponent == 0)
                return mantissa;

            const auto significand =
                static_cast<std::uint16_t>(0x10 | mantissa);

            return static_cast<std::uint16_t>(
                significand << (exponent - 1)
            );
        }

        std::uint8_t data;
    };
}
#endif //DSP_LOG_UINT_HPP
