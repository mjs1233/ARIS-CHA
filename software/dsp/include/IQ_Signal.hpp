//
// Created by takakura on 8/30/26.
//

#ifndef DSP_IQ_SIGNAL_HPP
#define DSP_IQ_SIGNAL_HPP
#include <cstdint>

namespace aris::dsp {
    template <typename T>
    struct IQ_Signal {

        T I;
        T Q;

        [[nodiscard]]
        int64_t power() const {
            int64_t i64_I = I;
            int64_t i64_Q = Q;
            return i64_I * i64_I + i64_Q * i64_Q;
        }
    };
}

#endif //DSP_IQ_SIGNAL_HPP
