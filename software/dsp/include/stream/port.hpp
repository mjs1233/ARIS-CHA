//
// Created by takakura on 9/3/26.
//

#ifndef DSP_PORT_HPP
#define DSP_PORT_HPP

#include <cassert>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include <stream/stream_buffer.hpp>
#include <details/graph_context.hpp>
#include <stream/node.hpp>

namespace aris::dsp {



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

        std::optional<stream_buffer<T,Capacity>> m_buffer;
    };

    template <typename T, std::size_t Capacity>
    class output_port : public port_base {
    public:
        using value_type = T;
        static constexpr std::size_t capacity = Capacity;

        explicit output_port(node* owner) :
        port_base(owner,direction::out) {}

        void connect(input_port<T,Capacity>& input) {

            if (m_next) {
                throw std::logic_error("input port already connected");
            }
            m_next = &input;

            details::graph_context::connect(owner(),input.owner());
        }

        void send(stream_buffer<T, Capacity>&& buffer) {

            assert(m_next != nullptr);
            assert(!m_next->m_buffer);

            m_next->m_buffer = std::move(buffer);
        }

    private:
        input_port<T,Capacity>* m_next = nullptr;
    };
}

#endif //DSP_PORT_HPP
