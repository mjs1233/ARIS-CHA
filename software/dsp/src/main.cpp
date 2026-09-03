#include <cstddef>
#include <cstdint>
#include <iostream>

#include "details/graph_context.hpp"
#include "iq_signal.hpp"
#include "stream/noise_gate/noise_gate.hpp"
#include "stream/power/iq_power.hpp"
#include "stream/separator.hpp"
#include "stream/sink/sink_kst.hpp"
#include "stream/source/iq_file_source.hpp"
#include "stream/stream_pool.hpp"

int main() {
    constexpr std::size_t stream_size = 4096;

    const auto iq_pool =
        aris::dsp::stream_pool_manager::add_pool<
            aris::dsp::iq_signal,
            stream_size
        >(2);
    const auto power_pool =
        aris::dsp::stream_pool_manager::add_pool<
            std::uint16_t,
            stream_size
        >(3);
    aris::dsp::stream_pool_manager::alloc_pools();

    aris::dsp::stage::modes1_source<stream_size> source{iq_pool};
    aris::dsp::stage::iq_power<stream_size> power{power_pool};
    aris::dsp::stage::separator<std::uint16_t, stream_size> separator{
        power_pool
    };

    using estimator = aris::dsp::stage::trailing_estimator<1024>;
    aris::dsp::stage::noise_gate<stream_size, estimator, 256> gate{
        power_pool
    };
    aris::dsp::stage::sink_kst<"modes1_power.dat"> sink;

    aris::dsp::details::graph_context::start();
    source.output.connect(power.iq_in);
    power.power_out.connect(separator.input);
    separator.output_a.connect(gate.sample_in);
    separator.output_b.connect(sink.source_in);
    gate.sample_out.connect(sink.result_in);
    aris::dsp::details::graph_context::finish();

    const auto execution_order = aris::dsp::details::graph_context::calc();
    while (!source.eof()) {
        for (auto* node : execution_order) {
            node->run();
        }
    }

    std::cerr
        << "samples=" << sink.written_samples()
        << " final_noise_floor=" << gate.noise_floor
        << '\n';
}
