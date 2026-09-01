//
// Created by takakura on 9/2/26.
//

#ifndef DSP_INTEGER_RESCALER_HPP
#define DSP_INTEGER_RESCALER_HPP
#include <cinttypes>
#include <stream/stream_buffer.hpp>

namespace aris::dsp {

    template <typename T,std::size_t N>
    struct integer_rescaler {
        void operator(stream::buffer<T,N>& in,) {

        }
    };
}

#endif //DSP_INTEGER_RESCALER_HPP
