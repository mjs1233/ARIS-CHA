#ifndef DSP_IQ_SIGNAL_HPP
#define DSP_IQ_SIGNAL_HPP
#include <cstdint>

namespace aris::dsp {
    struct iq_signal {
        std::int8_t I;
        std::int8_t Q;
    };
}


#endif //DSP_IQ_SIGNAL_HPP
