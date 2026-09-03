#ifndef DSP_IQ_POWER_HPP
#define DSP_IQ_POWER_HPP

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>

#include "iq_signal.hpp"
#include "stream/node.hpp"
#include "stream/port.hpp"
#include "stream/stream_pool.hpp"

namespace aris::dsp::stage {

template <std::size_t StreamSize>
class iq_power final : public node {
public:
    explicit iq_power(acquire_token<std::uint16_t, StreamSize> output_token)
        : m_output_token(output_token) {}

    input_port<iq_signal, StreamSize> iq_in{this};
    output_port<std::uint16_t, StreamSize> power_out{this};

    void run() override {
        auto input = iq_in.take();
        if (!input) {
            throw std::runtime_error("iq_power: input buffer required");
        }

        auto output = stream_pool_manager::acquire(m_output_token);
        if (!output) {
            throw std::runtime_error("iq_power: output buffer unavailable");
        }

        for (std::size_t i = 0; i < input->length; ++i) {
            const auto in_phase = static_cast<std::int32_t>(input->data[i].I);
            const auto quadrature = static_cast<std::int32_t>(input->data[i].Q);
            output->data[i] = static_cast<std::uint16_t>(
                in_phase * in_phase + quadrature * quadrature
            );
        }

        output->length = input->length;
        power_out.send(std::move(*output));
    }

private:
    acquire_token<std::uint16_t, StreamSize> m_output_token;
};

} // namespace aris::dsp::stage

#endif // DSP_IQ_POWER_HPP
