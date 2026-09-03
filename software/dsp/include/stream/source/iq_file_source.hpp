#ifndef DSP_IQ_FILE_SOURCE_HPP
#define DSP_IQ_FILE_SOURCE_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <utility>

#include "iq_signal.hpp"
#include "stream/node.hpp"
#include "stream/port.hpp"
#include "stream/stream_pool.hpp"

namespace aris::dsp::stage {

template <std::size_t StreamSize>
class iq_file_source : public node {
public:
    iq_file_source(acquire_token<iq_signal, StreamSize> token,
                   const std::filesystem::path& path)
        : m_token(token), m_file(path, std::ios::binary) {
        if (!m_file) {
            throw std::runtime_error(
                "iq_file_source: failed to open " + path.string()
            );
        }
    }

    output_port<iq_signal, StreamSize> output{this};

    void run() override {
        if (m_eof) {
            throw std::logic_error("iq_file_source: end of file reached");
        }

        auto buffer = stream_pool_manager::acquire(m_token);
        if (!buffer) {
            throw std::runtime_error("iq_file_source: output buffer unavailable");
        }

        m_file.read(
            reinterpret_cast<char*>(m_raw.data()),
            static_cast<std::streamsize>(m_raw.size())
        );
        const auto byte_count = static_cast<std::size_t>(m_file.gcount());

        if ((byte_count & 1U) != 0) {
            throw std::runtime_error(
                "iq_file_source: truncated I/Q pair at end of file"
            );
        }
        if (byte_count == 0) {
            m_eof = true;
            throw std::logic_error("iq_file_source: no samples remaining");
        }

        const auto sample_count = byte_count / 2;
        for (std::size_t i = 0; i < sample_count; ++i) {
            buffer->data[i] = iq_signal{
                .I = static_cast<std::int8_t>(
                    static_cast<std::int16_t>(m_raw[i * 2]) - 128
                ),
                .Q = static_cast<std::int8_t>(
                    static_cast<std::int16_t>(m_raw[i * 2 + 1]) - 128
                )
            };
        }

        buffer->length = sample_count;
        m_eof = byte_count < m_raw.size() ||
            m_file.peek() == std::char_traits<char>::eof();
        output.send(std::move(*buffer));
    }

    [[nodiscard]]
    bool eof() const noexcept {
        return m_eof;
    }

private:
    acquire_token<iq_signal, StreamSize> m_token;
    std::ifstream m_file;
    std::array<std::uint8_t, StreamSize * 2> m_raw{};
    bool m_eof = false;
};

template <std::size_t StreamSize>
class modes1_source final : public iq_file_source<StreamSize> {
public:
    explicit modes1_source(acquire_token<iq_signal, StreamSize> token)
        : iq_file_source<StreamSize>(token, resource_path()) {}

private:
    static std::filesystem::path resource_path() {
#ifdef ARIS_DSP_RESOURCE_DIR
        return std::filesystem::path{ARIS_DSP_RESOURCE_DIR} / "modes1.bin";
#else
        return std::filesystem::path{"res"} / "modes1.bin";
#endif
    }
};

} // namespace aris::dsp::stage

#endif // DSP_IQ_FILE_SOURCE_HPP
