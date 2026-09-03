//
// Created by takakura on 9/3/26.
//

#ifndef DSP_LOG_UINT_HPP
#define DSP_LOG_UINT_HPP
#include <bit>
#include <cstdint>


namespace aris::dsp {

    using log_uint8_t = std::uint8_t;
    struct log_uint8 {
        static log_uint8_t encode(std::uint16_t value) noexcept;
        static std::uint16_t decode(log_uint8_t data) noexcept;
    };

}
#endif //DSP_LOG_UINT_HPP
