#ifndef DSP_IQ_SIGNAL_HPP
#define DSP_IQ_SIGNAL_HPP
#include <cstdint>

namespace aris::dsp {
    struct iq_signal {
        uint16_t I;
        uint16_t Q;
    };
}


#endif //DSP_IQ_SIGNAL_HPP
