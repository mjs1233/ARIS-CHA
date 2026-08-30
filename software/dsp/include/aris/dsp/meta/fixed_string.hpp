//
// Created by takakura on 8/30/26.
//

#ifndef DSP_FIXED_STRING_HPP
#define DSP_FIXED_STRING_HPP
#include <cstddef>
#include <algorithm>
namespace aris::dsp::meta {
    template <std::size_t N>
    struct fixed_string {
        char value[N];

        constexpr fixed_string(const char (&str)[N]) {
            std::copy_n(str, N, value);
        }

        constexpr auto operator<=>(const fixed_string&) const = default;
    };
}
#endif //DSP_FIXED_STRING_HPP
