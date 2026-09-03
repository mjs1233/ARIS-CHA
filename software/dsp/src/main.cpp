#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <numbers>
#include <ostream>
#include <stdexcept>

#include "details/graph_context.hpp"
#include "stream/stream_node.hpp"
#include "stream/stream_pool.hpp"

namespace {

constexpr std::size_t stream_size = 4096;
using float_token = aris::dsp::acquire_token<float, stream_size>;

aris::dsp::float_stream<stream_size> acquire_stream(const float_token token) {
    auto buffer = aris::dsp::stream_pool_manager::acquire(token);
    if (!buffer) {
        throw std::runtime_error("failed to acquire a float stream");
    }
    return std::move(*buffer);
}

class constant_source final : public aris::dsp::node {
public:
    constant_source(const float value, const float_token token)
        : m_value(value), m_token(token) {}

    aris::dsp::output_port<float, stream_size> output{this};

    void run() override {
        auto buffer = acquire_stream(m_token);
        std::fill_n(buffer.data, stream_size, m_value);
        buffer.length = stream_size;
        output.send(std::move(buffer));
    }

private:
    float m_value;
    float_token m_token;
};

template <std::size_t SampleRate>
class sine_oscillator final : public aris::dsp::node {
public:
    explicit sine_oscillator(const float_token token) : m_token(token) {}

    aris::dsp::input_port<float, stream_size> frequency{this};
    aris::dsp::input_port<float, stream_size> amplitude{this};
    aris::dsp::output_port<float, stream_size> output{this};

    void run() override {
        auto frequency_stream = frequency.take();
        auto amplitude_stream = amplitude.take();
        if (!frequency_stream || !amplitude_stream) {
            throw std::logic_error("sine oscillator input is missing");
        }

        const auto length = std::min(frequency_stream->length,
                                     amplitude_stream->length);
        auto result = acquire_stream(m_token);
        constexpr float two_pi = 2.0F * std::numbers::pi_v<float>;

        for (std::size_t i = 0; i < length; ++i) {
            result.data[i] = amplitude_stream->data[i] * std::sin(m_phase);
            m_phase += two_pi * frequency_stream->data[i] /
                       static_cast<float>(SampleRate);
            m_phase = std::fmod(m_phase, two_pi);
        }

        result.length = length;
        output.send(std::move(result));
    }

private:
    float_token m_token;
    float m_phase = 0.0F;
};

class add_and_threshold final : public aris::dsp::node {
public:
    add_and_threshold(const float threshold, const float_token token)
        : m_threshold(threshold), m_token(token) {}

    aris::dsp::input_port<float, stream_size> lhs{this};
    aris::dsp::input_port<float, stream_size> rhs{this};
    aris::dsp::output_port<float, stream_size> output{this};

    void run() override {
        auto left = lhs.take();
        auto right = rhs.take();
        if (!left || !right) {
            throw std::logic_error("adder input is missing");
        }

        const auto length = std::min(left->length, right->length);
        auto result = acquire_stream(m_token);
        for (std::size_t i = 0; i < length; ++i) {
            const float sum = left->data[i] + right->data[i];
            result.data[i] = sum < m_threshold ? 0.0F : sum;
        }

        result.length = length;
        output.send(std::move(result));
    }

private:
    float m_threshold;
    float_token m_token;
};

class ostream_sink final : public aris::dsp::node {
public:
    explicit ostream_sink(std::ostream& stream) : m_stream(stream) {}

    aris::dsp::input_port<float, stream_size> input{this};

    void run() override {
        auto buffer = input.take();
        if (!buffer) {
            throw std::logic_error("sink input is missing");
        }

        for (std::size_t i = 0; i < buffer->length; ++i) {
            m_stream << i << '\t' << buffer->data[i] << '\n';
        }
    }

private:
    std::ostream& m_stream;
};

} // namespace

int main() {
    constexpr std::size_t sample_rate = 48'000;

    const auto float_pool =
        aris::dsp::stream_pool_manager::add_pool<float, stream_size>(8);
    aris::dsp::stream_pool_manager::alloc_pools();

    constant_source frequency_a{440.0F, float_pool};
    constant_source amplitude_a{0.8F, float_pool};
    constant_source frequency_b{660.0F, float_pool};
    constant_source amplitude_b{0.4F, float_pool};
    sine_oscillator<sample_rate> oscillator_a{float_pool};
    sine_oscillator<sample_rate> oscillator_b{float_pool};
    add_and_threshold adder{0.25F, float_pool};
    ostream_sink sink{std::cout};

    aris::dsp::details::graph_context::start();
    frequency_a.output.connect(oscillator_a.frequency);
    amplitude_a.output.connect(oscillator_a.amplitude);
    frequency_b.output.connect(oscillator_b.frequency);
    amplitude_b.output.connect(oscillator_b.amplitude);
    oscillator_a.output.connect(adder.lhs);
    oscillator_b.output.connect(adder.rhs);
    adder.output.connect(sink.input);
    aris::dsp::details::graph_context::finish();

    const auto execution_order = aris::dsp::details::graph_context::calc();
    if (execution_order.size() != 8) {
        throw std::logic_error("unexpected DSP graph size");
    }

    for (auto* node : execution_order) {
        node->run();
    }

    return 0;
}
