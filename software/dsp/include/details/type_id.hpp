//
// Created by takakura on 9/2/26.
//
#ifndef DSP_TYPE_ID_HPP
#define DSP_TYPE_ID_HPP

#include <atomic>
#include <cstddef>

namespace aris::dsp::details {

    using type_id_t = std::size_t;

    inline type_id_t next_type_id() {
        static std::atomic<type_id_t> counter{0};
        return counter++;
    }

    template <typename T>
    type_id_t type_id() {
        static const type_id_t id = next_type_id();
        return id;
    }
}

#endif //DSP_TYPE_ID_HPP
