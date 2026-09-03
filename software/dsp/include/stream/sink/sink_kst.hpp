#ifndef DSP_SINK_KST_HPP
#define DSP_SINK_KST_HPP

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <type_traits>

#include "stream/node.hpp"
#include "stream/port.hpp"
#include "types.hpp"

namespace aris::dsp::details {

template <std::size_t N>
struct fixed_string {
    char data[N]{};

    consteval fixed_string(const char (&value)[N]) {
        for (std::size_t i = 0; i < N; ++i) {
            data[i] = value[i];
        }
    }

    [[nodiscard]]
    static consteval std::size_t size() noexcept {
        return N - 1;
    }
};

} // namespace aris::dsp::details

namespace aris::dsp::stage {

template <details::fixed_string FileName,
          typename T = sample_type_t,
          std::size_t StreamSize = 4096>
requires std::is_arithmetic_v<T>
class sink_kst final : public node {
public:
    static_assert(FileName.size() > 0, "KST output filename must not be empty");

    sink_kst() : m_stream(FileName.data) {
        if (!m_stream) {
            throw std::runtime_error("sink_kst: failed to open output file");
        }

    }

    input_port<T, StreamSize> source_in{this};
    input_port<T, StreamSize> result_in{this};

    void run() override {
        auto source = source_in.take();
        auto result = result_in.take();
        if (!source || !result) {
            throw std::runtime_error("sink_kst: source and result buffers required");
        }

        const auto length = std::min(source->length, result->length);
        for (std::size_t i = 0; i < length; ++i) {
            m_stream << m_sample_index++
                     << '\t' << source->data[i]
                     << '\t' << result->data[i]
                     << '\n';
        }

        m_stream.flush();
        if (!m_stream) {
            throw std::runtime_error("sink_kst: failed to write output file");
        }
    }

    [[nodiscard]]
    std::size_t written_samples() const noexcept {
        return m_sample_index;
    }

private:
    std::ofstream m_stream;
    std::size_t m_sample_index = 0;
};

} // namespace aris::dsp::stage

#endif // DSP_SINK_KST_HPP
