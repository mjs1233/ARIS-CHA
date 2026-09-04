#ifndef DSP_STREAM_GRAPH_HPP
#define DSP_STREAM_GRAPH_HPP

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <queue>
#include <ranges>
#include <stdexcept>
#include <vector>

#include "stream/node.hpp"
#include "stream/port.hpp"

namespace aris::dsp {

class graph {
public:
    template <typename T, std::size_t Capacity>
    void connect(output_port<T, Capacity>& output,
                 input_port<T, Capacity>& input) {
        if (output.m_next != nullptr) {
            throw std::logic_error("output port already connected");
        }
        if (input.m_connected) {
            throw std::logic_error("input port already connected");
        }

        add_dependency(output.owner(), input.owner());
        output.m_next = &input;
        input.m_connected = true;
    }

    [[nodiscard]]
    std::vector<node*> compile() const {
        std::vector<std::uint32_t> indegree;
        indegree.reserve(m_nodes.size());

        for (const auto& data : m_nodes) {
            indegree.push_back(data.indegree);
        }

        std::queue<std::size_t> ready;
        for (std::size_t i = 0; i < m_nodes.size(); ++i) {
            if (indegree[i] == 0) {
                ready.push(i);
            }
        }

        std::vector<node*> result;
        result.reserve(m_nodes.size());

        while (!ready.empty()) {
            const auto current = ready.front();
            ready.pop();
            result.push_back(m_nodes[current].ptr);

            for (const auto next : m_nodes[current].next) {
                if (--indegree[next] == 0) {
                    ready.push(next);
                }
            }
        }

        if (result.size() != m_nodes.size()) {
            throw std::logic_error("graph: cycle detected");
        }

        return result;
    }

private:
    struct node_data {
        node* ptr;
        std::uint32_t indegree;
        std::vector<std::size_t> next;
    };

    static constexpr std::size_t npos = static_cast<std::size_t>(-1);

    void add_dependency(node* start, node* end) {
        if (start == nullptr || end == nullptr) {
            throw std::invalid_argument("graph: null node connection");
        }

        const auto start_index = ensure_node(start);
        const auto end_index = ensure_node(end);
        auto& next = m_nodes[start_index].next;

        if (std::ranges::find(next, end_index) != next.end()) {
            return;
        }

        next.push_back(end_index);
        ++m_nodes[end_index].indegree;
    }

    [[nodiscard]]
    std::size_t find_index(node* target) const noexcept {
        const auto iter = std::ranges::find(m_nodes, target, &node_data::ptr);
        if (iter == m_nodes.end()) {
            return npos;
        }

        return static_cast<std::size_t>(std::distance(m_nodes.begin(), iter));
    }

    [[nodiscard]]
    std::size_t ensure_node(node* target) {
        const auto index = find_index(target);
        if (index != npos) {
            return index;
        }

        m_nodes.push_back({.ptr = target, .indegree = 0, .next = {}});
        return m_nodes.size() - 1;
    }

    std::vector<node_data> m_nodes;
};

} // namespace aris::dsp

#endif // DSP_STREAM_GRAPH_HPP
