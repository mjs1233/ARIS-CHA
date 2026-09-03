#ifndef DSP_STREAM_SEPARATOR_HPP
#define DSP_STREAM_SEPARATOR_HPP

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <type_traits>

#include "stream/node.hpp"
#include "stream/port.hpp"
#include "stream/stream_pool.hpp"

namespace aris::dsp::stage {

template <typename T, std::size_t StreamSize>
requires std::is_trivial_v<T>
class separator final : public node {
public:
    explicit separator(acquire_token<T, StreamSize> token)
        : m_token(token) {}

    input_port<T, StreamSize> input{this};
    output_port<T, StreamSize> output_a{this};
    output_port<T, StreamSize> output_b{this};

    void run() override {
        auto source = input.take();
        if (!source) {
            throw std::runtime_error("separator: input buffer required");
        }

        auto first = stream_pool_manager::acquire(m_token);
        auto second = stream_pool_manager::acquire(m_token);
        if (!first || !second) {
            throw std::runtime_error("separator: output buffers unavailable");
        }

        std::copy_n(source->data, source->length, first->data);
        std::copy_n(source->data, source->length, second->data);
        first->length = source->length;
        second->length = source->length;

        output_a.send(std::move(*first));
        output_b.send(std::move(*second));
    }

private:
    acquire_token<T, StreamSize> m_token;
};

} // namespace aris::dsp::stage

#endif // DSP_STREAM_SEPARATOR_HPP
