//
// Created by takakura on 8/30/26.
//

#ifndef DSP_STREAM_BLOCK_HPP
#define DSP_STREAM_BLOCK_HPP
#include <vector>
namespace aris::dsp {
    template <typename T>
    struct stream_block {
        using type = T;
        std::vector<type> data;
    };
}
#endif //DSP_STREAM_BLOCK_HPP
