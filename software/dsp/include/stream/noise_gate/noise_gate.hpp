//
// Created by takakura on 9/2/26.
//

#ifndef DSP_NOISE_GATE_HPP
#define DSP_NOISE_GATE_HPP
#include <algorithm>
#include <cstddef>
#include <array>
#include <concepts>
#include <cstdint>
#include <optional>
#include <limits>
#include "types.hpp"
#include "log_uint.hpp"
#include "stream/node.hpp"
#include "stream/port.hpp"
#include "stream/stream_pool.hpp"

namespace aris::dsp::stage {

    template <typename T>
    concept noise_floor_estimator = requires (T t, const T ct, std::uint16_t in)
    {
        { t.push(in) } -> std::same_as<void>;
        { ct.calc() } -> std::same_as<std::optional<std::uint16_t>>;
        { ct.ready() } -> std::same_as<bool>;
        { ct.collected() } -> std::same_as<std::size_t>;
    };

    template <std::size_t Stream_Size,noise_floor_estimator Estimator,std::size_t Estimator_Update_Interval>
    requires (Estimator_Update_Interval > 0)
    struct noise_gate : node {

        input_port<std::uint16_t, Stream_Size> sample_in{this};
        output_port<std::uint16_t, Stream_Size> sample_out{this};

        acquire_token<std::uint16_t, Stream_Size> sample_out_token;

        std::size_t interval_index = 0;
        std::uint16_t noise_floor = 0;
        Estimator estimator;

        explicit noise_gate(acquire_token<std::uint16_t, Stream_Size> token)
            : sample_out_token(token) {}

        void run() override {

            auto input_sample = sample_in.take();

            if (!input_sample) {
                throw std::runtime_error("noise gate: input sample required");
            }

            auto output_sample = stream_pool_manager::acquire(sample_out_token);
            if (!output_sample) {
                throw std::runtime_error("noise gate: output buffer unavailable");
            }

            for (std::size_t i = 0; i < input_sample->length; ++i) {

                const auto sample = input_sample->data[i];
                estimator.push(sample);

                ++interval_index;
                if (interval_index >= Estimator_Update_Interval) {
                    interval_index = 0;

                    if (const auto estimate = estimator.calc()) {
                        constexpr std::uint32_t threshold_multiplier = 5;
                        const auto threshold =
                            static_cast<std::uint32_t>(*estimate) *
                            threshold_multiplier;
                        noise_floor = static_cast<std::uint16_t>(std::min(
                            threshold,
                            static_cast<std::uint32_t>(
                                std::numeric_limits<std::uint16_t>::max()
                            )
                        ));
                    }
                }

                output_sample->data[i] =
                    sample < noise_floor ? 0 : sample;
            }

            output_sample->length = input_sample->length;
            sample_out.send(std::move(*output_sample));
        }
    };

    template <std::size_t N>
    requires (0 < N && N <= 1024)
    struct trailing_estimator {
        std::array<std::uint16_t, 256> hist {};
        std::array<log_uint8_t, N> queue {};

        std::size_t head = 0;
        std::size_t count = 0;

        void push(std::uint16_t in) {
            const auto encoded = log_uint8::encode(in);

            if (ready()) {
                --hist[queue[head]];
            } else {
                ++count;
            }

            queue[head] = encoded;
            ++hist[encoded];
            head = (head + 1) % N;
        }

        [[nodiscard]]
        bool ready() const noexcept {
            return count == N;
        }

        [[nodiscard]]
        std::size_t collected() const noexcept {
            return count;
        }

        [[nodiscard]]
        std::optional<std::uint16_t> calc() const noexcept {
            if (!ready()) {
                return std::nullopt;
            }

            static constexpr std::size_t percentile_rank =
                (N * 30 + 99) / 100;

            std::size_t cumulative_count = 0;
            for (std::size_t i = 0; i < hist.size(); ++i) {
                cumulative_count += hist[i];
                if (cumulative_count >= percentile_rank) {
                    return log_uint8::decode(static_cast<log_uint8_t>(i));
                }
            }

            return std::nullopt;
        }

    };
}

#endif //DSP_NOISE_GATE_HPP
