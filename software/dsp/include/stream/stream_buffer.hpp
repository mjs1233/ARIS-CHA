//
// Created by takakura on 9/2/26.
//

#ifndef DSP_STREAM_BUFFER_HPP
#define DSP_STREAM_BUFFER_HPP
#include <cstddef>
#include <type_traits>
#include "details/stream_pool_fwd.hpp"
#include "iq_signal.hpp"

namespace aris::dsp {

    class stream_pool_manager;

    template <typename T,std::size_t Capacity>
    requires std::is_trivial_v<T>
    struct stream_buffer {
        using value_type = T;
        static constexpr std::size_t capacity = Capacity;
        T* data {};
        std::size_t length {};

        stream_buffer() = delete;
        ~stream_buffer() {
            if (m_storage != nullptr) {
                details::release_stream_buffer_data(m_pool_id, m_storage);
            }
        }

        stream_buffer(const stream_buffer&) = delete;
        stream_buffer& operator=(const stream_buffer&) = delete;

        stream_buffer(stream_buffer&& other) noexcept {

            data = other.data;
            length = other.length;
            m_pool_id = other.m_pool_id;
            m_storage = other.m_storage;
            other.data = nullptr;
            other.length = 0;
            other.m_pool_id = 0;
            other.m_storage = nullptr;

        }
        stream_buffer& operator=(stream_buffer&& other) noexcept {

            if (&other == this) {
                return *this;
            }

            if (m_storage != nullptr) {
                details::release_stream_buffer_data(m_pool_id, m_storage);
            }
            data = other.data;
            length = other.length;
            m_pool_id = other.m_pool_id;
            m_storage = other.m_storage;
            other.data = nullptr;
            other.length = 0;
            other.m_pool_id = 0;
            other.m_storage = nullptr;

            return *this;
        }

    private:
        stream_buffer(std::size_t pool_id, void* ptr) :
        data(static_cast<T*>(ptr)),
        m_pool_id(pool_id),
        m_storage(ptr) {}

        std::size_t m_pool_id {};
        void* m_storage {};

        friend class stream_pool_manager;
    };

    template <std::size_t Capacity>
    using iq_stream = stream_buffer<iq_signal,Capacity>;
    template <std::size_t Capacity>
    using float_stream = stream_buffer<float,Capacity>;

}

#endif //DSP_STREAM_BUFFER_HPP
