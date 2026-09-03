//
// Created by takakura on 9/3/26.
//

#ifndef DSP_STREAM_POOL_FWD_HPP
#define DSP_STREAM_POOL_FWD_HPP
#include <cstdint>
namespace aris::dsp::details {
    void acquire_stream_buffer_data();
    void release_stream_buffer_data(std::size_t pool_id, void* ptr);
}

#endif //DSP_STREAM_POOL_FWD_HPP
