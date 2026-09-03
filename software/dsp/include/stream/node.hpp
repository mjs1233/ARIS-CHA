//
// Created by takakura on 9/3/26.
//

#ifndef DSP_NODE_HPP
#define DSP_NODE_HPP
#include <vector>
namespace aris::dsp {
    class node {
    public:
        node(const node &) = delete;
        node &operator=(const node &) = delete;

        virtual ~node() = default;
        virtual void run() = 0;



        std::vector<node*> next;

    protected:
        node() = default;
    };
}
#endif //DSP_NODE_HPP
