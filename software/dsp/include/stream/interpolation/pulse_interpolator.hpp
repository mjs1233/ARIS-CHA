#ifndef DSP_PULSE_INTERPOLATOR_HPP
#define DSP_PULSE_INTERPOLATOR_HPP

#include <cstddef>
#include <cstdint>

#include "stream/node.hpp"
#include "stream/port.hpp"
#include "stream/stream_pool.hpp"

namespace aris::dsp::stage {

template <std::size_t StreamSize, std::size_t InterpolationPrecision>
requires (InterpolationPrecision >= 2 && InterpolationPrecision <= 16)
class pulse_interpolator final : public node {
public:
    pulse_interpolator(
        acquire_token<std::uint16_t, StreamSize> magnitude_token,
        acquire_token<std::uint8_t, StreamSize> shift_token
    ) : m_magnitude_token(magnitude_token),
        m_shift_token(shift_token) {}

    input_port<std::uint16_t, StreamSize> magnitude_in{this};
    output_port<std::uint16_t, StreamSize> magnitude_out{this};
    output_port<std::uint8_t, StreamSize> shift_out{this};

    void run() override {

    }

private:
    [[nodiscard]]
    std::uint16_t interpolate_magnitude(
        std::uint16_t previous,
        std::uint16_t current,
        std::uint8_t shift
    ) const noexcept;

    acquire_token<std::uint16_t, StreamSize> m_magnitude_token;
    acquire_token<std::uint8_t, StreamSize> m_shift_token;

    std::uint16_t m_previous_magnitude = 0;
    bool m_has_previous_magnitude = false;
};

} // namespace aris::dsp::stage

#endif // DSP_PULSE_INTERPOLATOR_HPP
