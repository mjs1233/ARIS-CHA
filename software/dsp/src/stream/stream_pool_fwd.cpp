//
// Created by takakura on 9/3/26.
//
#include "stream/stream_pool.hpp"
namespace aris::dsp::details {

    void release_stream_buffer_data(std::size_t pool_id, void* ptr) {
        stream_pool_manager::release(pool_id, ptr);
    }
}