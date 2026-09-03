//
// Created by takakura on 9/3/26.
//

#ifndef DSP_GRAPH_CONTEXT_HPP
#define DSP_GRAPH_CONTEXT_HPP

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <queue>
#include <ranges>
#include <stdexcept>
#include <vector>

#include "stream/node.hpp"

namespace aris::dsp::details {

class graph_context {
public:
    static void start() {
        if (m_started) {
            throw std::logic_error(
                "[graph_context] nested graph context is not supported"
            );
        }

        m_nodes.clear();
        m_started = true;
    }

    static void finish() {
        if (!m_started) {
            throw std::logic_error(
                "[graph_context] graph context is not started"
            );
        }

        m_started = false;
    }


    static void connect(node* start, node* end) {
        require_started();

        if (start == nullptr || end == nullptr) {
            throw std::invalid_argument(
                "[graph_context] null node connection"
            );
        }

        auto start_index = ensure_node(start);
        auto end_index   = ensure_node(end);

        auto& next = m_nodes[start_index].next;

        // Same node dependency already exists.
        if (std::ranges::find(next, end_index) != next.end()) {
            return;
        }

        next.push_back(end_index);
        ++m_nodes[end_index].indegree;
    }

    [[nodiscard]]
    static std::vector<node*> calc() {
        if (m_started) {
            throw std::logic_error(
                "[graph_context] graph context is not finished"
            );
        }

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
            throw std::logic_error(
                "[graph_context] cycle detected"
            );
        }

        return result;
    }

    [[nodiscard]]
    static bool started() noexcept {
        return m_started;
    }

private:
    struct node_data {
        node* ptr;
        std::uint32_t indegree;
        std::vector<std::size_t> next;
    };

    static constexpr std::size_t npos =
        static_cast<std::size_t>(-1);

    static void require_started() {
        if (!m_started) {
            throw std::logic_error(
                "[graph_context] graph context is not started"
            );
        }
    }

    [[nodiscard]]
    static std::size_t find_index(node* n) noexcept {
        auto iter = std::ranges::find(
            m_nodes,
            n,
            &node_data::ptr
        );

        if (iter == m_nodes.end()) {
            return npos;
        }

        return static_cast<std::size_t>(
            std::distance(m_nodes.begin(), iter)
        );
    }

    [[nodiscard]]
    static std::size_t ensure_node(node* n) {
        auto index = find_index(n);

        if (index != npos) {
            return index;
        }

        m_nodes.push_back({
            .ptr = n,
            .indegree = 0,
            .next = {}
        });

        return m_nodes.size() - 1;
    }

private:
    inline static bool m_started = false;
    inline static std::vector<node_data> m_nodes;
};

} // namespace aris::dsp::details

#endif // DSP_GRAPH_CONTEXT_HPP