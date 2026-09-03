//
// Created by takakura on 9/2/26.
//

#ifndef DSP_NOISE_GATE_HPP
#define DSP_NOISE_GATE_HPP
#include <cstddef>
#include <array>
#include "types.hpp"

namespace aris::dsp::stream {

    template <typename T>
    concept noise_floor_estimator = requires (T t)
    {
        {t.calc()};
        {t.reset()};
    };

    template <noise_floor_estimator Estimator>
    struct noise_gate {

    };

    template <std::size_t N>
    requires (N <= 1024)
    struct trailing_estimator {

    };
}

#endif //DSP_NOISE_GATE_HPP
