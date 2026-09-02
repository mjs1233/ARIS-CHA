//
// Created by takakura on 9/2/26.
//

#ifndef DSP_STREAM_BUFFER_HPP
#define DSP_STREAM_BUFFER_HPP
#include <array>
#include <cstdint>
#include "details/stream_pool_fwd.hpp"
#include "iq_signal.hpp"

namespace aris::dsp {

    template <typename T>
    struct stream_buffer {
        using value_type = T;
        std::size_t capacity {};
        T* data {};
        std::size_t pool_id {};
        std::size_t length {};

        stream_buffer(std::size_t capacity,std::size_t pool_id, void* ptr) : capacity(capacity),pool_id(pool_id),data(static_cast<T*>(ptr)) {}
        ~stream_buffer() {
            details::release_stream_buffer_data(pool_id,data);
        }

        stream_buffer(const stream_buffer&) = delete;
        stream_buffer& operator=(const stream_buffer&) = delete;
        stream_buffer(stream_buffer&&) noexcept = default;
        stream_buffer& operator=(stream_buffer&&) noexcept = default;
    };


    using iq_stream = stream_buffer<iq_signal>;

}

#endif //DSP_STREAM_BUFFER_HPP
