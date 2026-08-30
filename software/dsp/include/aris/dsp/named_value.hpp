//
// Created by takakura on 8/30/26.
//

#ifndef DSP_NAMED_VALUE_HPP
#define DSP_NAMED_VALUE_HPP
#include "meta/fixed_string.hpp"
namespace aris::dsp {
    template <typename T, meta::fixed_string Name>
    struct named_value {
        using value_type = T;

        static constexpr auto name = Name;

        T value{};
    };
}
#endif //DSP_NAMED_VALUE_HPP
