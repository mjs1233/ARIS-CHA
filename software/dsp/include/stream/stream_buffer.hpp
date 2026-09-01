//
// Created by takakura on 9/2/26.
//

#ifndef DSP_STREAM_BUFFER_HPP
#define DSP_STREAM_BUFFER_HPP
#include <array>
#include <cstdint>

#include "iq_signal.hpp"

namespace aris::dsp {

    template <typename T>
    struct stream_buffer {
        using value_type = T;
        std::size_t capacity;
        T* data;
        std::size_t length;
    };

    using iq_stream = stream_buffer<iq_signal>;

}

#endif //DSP_STREAM_BUFFER_HPP
