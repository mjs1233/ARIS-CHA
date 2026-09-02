//
// Created by takakura on 9/3/26.
//

#ifndef DSP_MEMORY_ALIGN_HPP
#define DSP_MEMORY_ALIGN_HPP
#include <cstdint>
#include <bit>
#include <stdexcept>

namespace aris::dsp::details {

    constexpr std::size_t align_up(std::size_t x,std::size_t alignment) {

        if (!std::has_single_bit(alignment)) {
            throw std::invalid_argument("alignment has to be power of 2");
        }

        return (x + alignment - 1) & ~(alignment - 1);
    }

    template <std::size_t Alignment>
    struct aligned_delete {
        void operator()(std::byte* ptr) const noexcept {
            ::operator delete[](ptr, std::align_val_t{Alignment});
        }
    };
}
#endif //DSP_MEMORY_ALIGN_HPP
