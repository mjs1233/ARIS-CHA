//
// Created by takakura on 9/3/26.
//

#ifndef DSP_PORT_HPP
#define DSP_PORT_HPP

#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include <stream/stream_buffer.hpp>
#include <stream/node.hpp>

namespace aris::dsp {

    class graph;

    class port_base {
    public:
        enum class direction {
            in, out
        };

        node* owner() const noexcept {
            return m_owner;
        }

        direction dir() const noexcept {
            return m_direction;
        }

    protected:
        explicit port_base(node* owner, direction dir_) :
        m_owner(owner), m_direction(dir_) {}

    private:
        node* m_owner;
    protected:
        direction m_direction;
    };

    template <typename T, std::size_t Capacity>
    class output_port;

    template <typename T, std::size_t Capacity>
    class input_port : public port_base {
    public:
        using value_type = T;
        static constexpr std::size_t capacity = Capacity;

        explicit input_port(node* owner) :
        port_base(owner, direction::in) {}

        std::optional<stream_buffer<T,Capacity>> take() noexcept {
            return std::exchange(m_buffer, std::nullopt);
        }

    private:
        friend class output_port<T,Capacity>;
        friend class graph;

        std::optional<stream_buffer<T,Capacity>> m_buffer;
        bool m_connected = false;
    };

    template <typename T, std::size_t Capacity>
    class output_port : public port_base {
    public:
        using value_type = T;
        static constexpr std::size_t capacity = Capacity;

        explicit output_port(node* owner) :
        port_base(owner,direction::out) {}

        void send(stream_buffer<T, Capacity>&& buffer) {

            if (m_next == nullptr) {
                throw std::logic_error("output port is not connected");
            }
            if (m_next->m_buffer) {
                throw std::logic_error("input port already contains a buffer");
            }

            m_next->m_buffer = std::move(buffer);
        }

    private:
        friend class graph;

        input_port<T,Capacity>* m_next = nullptr;
    };
}

#endif //DSP_PORT_HPP
