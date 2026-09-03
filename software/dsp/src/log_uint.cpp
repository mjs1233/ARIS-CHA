#include "log_uint.hpp"
namespace aris::dsp {

    log_uint8_t log_uint8::encode(uint16_t value) noexcept {

        if (value < 16) {
            return static_cast<std::uint8_t>(value);
        }

        const auto msb = std::bit_width(value) - 1;

        const auto exponent = msb - 3;
        const auto mantissa =
            (value >> (msb - 4)) & 0b00001111;

        return static_cast<std::uint8_t>(
            (exponent << 4) | mantissa
        );
    }

    std::uint16_t log_uint8::decode(log_uint8_t data) noexcept {
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
}
