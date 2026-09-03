//
// Created by takakura on 9/2/26.
//

#ifndef DSP_STREAM_POOL_HPP
#define DSP_STREAM_POOL_HPP
#include <cstdint>
#include <expected>
#include <memory>
#include <stack>
#include <vector>
#include <cstddef>
#include <new>
#include <stdexcept>

#include "stream_buffer.hpp"
#include "details/type_id.hpp"
#include "details/memory_align.hpp"

namespace aris::dsp {

    template<typename T, std::size_t Capacity>
    class acquire_token {
    public:
        using type = T;
        static constexpr std::size_t capacity = Capacity;

    private:
        std::size_t m_pool_id;

        explicit acquire_token(std::size_t id)
            : m_pool_id(id) {}

        friend class stream_pool_manager;
    };

    class stream_pool_manager {
    private:
        struct pool_data {
            details::type_id_t type_id;
            std::size_t type_size;
            std::size_t capacity;
            std::size_t count;
            std::size_t offset;
            std::size_t stride;

            //TODO) flatten & combine these 2 memory area.
            std::stack<std::size_t> free_list;
            std::vector<uint64_t> free_slots;
        };

    public:
        enum class acquire_fail {
            no_free_slot,
            not_allocated
        };



        template <typename T,std::size_t Capacity>
        static acquire_token<T,Capacity> add_pool(std::size_t Count) {

            static_assert(alignof(T) <= pool_alignment);

            if (get_instance()->data)
                throw std::runtime_error("pool already allocated");

            get_instance()->pools.push_back(
              pool_data {
                .type_id = details::type_id<T>(),
                .type_size = sizeof(T),
                .capacity = Capacity,
                .count = Count,
                .offset = 0,
                .stride = 0
            });
            return acquire_token<T,Capacity> { get_instance()->pools.size() - 1 };
        }

        static void alloc_pools() {

            auto self = get_instance();

            if (self->data) {
                throw std::runtime_error("pool already allocated");
            }

            self->data_size = 0;
            auto& pools = self->pools;

            for (auto& pool : pools) {
                pool.offset = self->data_size;
                pool.stride = details::align_up(pool.type_size * pool.capacity, pool_alignment);

                self->data_size += pool.stride * pool.count;
                //reset free list
                while (!pool.free_list.empty()) {
                    pool.free_list.pop();
                }

                for (std::size_t idx = 0; idx < pool.count; ++idx) {
                    pool.free_list.push(idx);
                }


                pool.free_slots.assign((pool.count + 63) / 64,0);
            }


            self->data.reset(
                static_cast<std::byte*>(::operator new[](self->data_size, std::align_val_t{64}))
            );
        }

        template <typename T,std::size_t Capacity>
        static std::expected<stream_buffer<T,Capacity>,acquire_fail> acquire(acquire_token<T,Capacity> token) {

            auto self = get_instance();

            if (!self->data) {
                return std::unexpected(acquire_fail::not_allocated);
            }

            void* ptr = self->acquire_impl(token.m_pool_id);
            if (ptr == nullptr) {
                return std::unexpected(acquire_fail::no_free_slot);
            }
            return stream_buffer<T,Capacity>{token.m_pool_id,ptr};
        }

        static stream_pool_manager* get_instance() {
            static stream_pool_manager instance;
            return &instance;
        }
    private:

        static void release(size_t pool_id, void* ptr) {

            auto self = get_instance();

            if (!self->data) {
                return;
            }

            if (pool_id >= self->pools.size()) {
                return;
            }

            auto& pool = self->pools[pool_id];

            auto* base = self->data.get() + pool.offset;
            auto* p    = static_cast<std::byte*>(ptr);

            const std::ptrdiff_t byte_offset = p - base;

            if (byte_offset < 0)
                return;

            if (byte_offset % (pool.stride) != 0)
                return;


            const std::size_t index =
                static_cast<std::size_t>(byte_offset) / pool.stride;

            if (index >= pool.count)
                return;

            if (!space_slot(index, pool.free_slots)) {
                return;
            }

            pool.free_list.push(index);
        }

        void* acquire_impl(std::size_t pool_id) {

            auto& pool = get_instance()->pools[pool_id];

            if (pool.free_list.empty()) {
                return nullptr;
            }
            const auto index = pool.free_list.top();

            if (mark_slot(index, pool.free_slots)) {
                throw std::runtime_error("corrupted free list");
            }

            std::byte* pos =
                data.get() + pool.offset + index * pool.stride;

            pool.free_list.pop();

            return pos;
        }

        static bool mark_slot(std::size_t slot_idx, std::vector<std::uint64_t>& slots) {

            uint32_t slot_block_idx = slot_idx / 64;
            uint32_t slot_block_offset = slot_idx % 64;
            bool prev = slots[slot_block_idx] & (1ull << slot_block_offset);
            slots[slot_block_idx] |= (1ull << slot_block_offset);

            return prev;
        }

        static bool space_slot(std::size_t slot_idx, std::vector<std::uint64_t>& slots) {

            uint32_t slot_block_idx = slot_idx / 64;
            uint32_t slot_block_offset = slot_idx % 64;
            bool prev = slots[slot_block_idx] & (1ull << slot_block_offset);
            slots[slot_block_idx] &= ~(1ull << slot_block_offset);

            return prev;
        }
        static constexpr size_t pool_alignment = 64;

        std::vector<pool_data> pools;
        std::size_t data_size = 0;
        std::unique_ptr<std::byte[],details::aligned_delete<pool_alignment>> data = nullptr;

        friend void details::release_stream_buffer_data(size_t, void*);
    };
}
#endif //DSP_STREAM_POOL_HPP
