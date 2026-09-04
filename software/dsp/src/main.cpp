#include <cstddef>
#include <cstdint>
#include <iostream>

#include "iq_signal.hpp"
#include "stream/graph.hpp"
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

    aris::dsp::graph pipeline;
    pipeline.connect(source.output, power.iq_in);
    pipeline.connect(power.power_out, separator.input);
    pipeline.connect(separator.output_a, gate.sample_in);
    pipeline.connect(separator.output_b, sink.source_in);
    pipeline.connect(gate.sample_out, sink.result_in);

    const auto execution_order = pipeline.compile();
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
