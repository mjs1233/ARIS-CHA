//
// Created by takakura on 9/2/26.
//

#ifndef DSP_STREAM_POOL_HPP
#define DSP_STREAM_POOL_HPP
#include <cstdint>
#include <expected>
#include <vector>

#include "stream_buffer.hpp"
#include "details/type_id.hpp"

namespace aris::dsp {

    template<typename T, std::size_t Capacity>
    class acquire_token {
    public:
        using type = T;
        static constexpr std::size_t capacity = Capacity;

    private:
        std::size_t pool_id_;

        explicit acquire_token(std::size_t id)
            : pool_id_(id) {}

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

            std::size_t left = 0;

        };

    public:
        enum class acquire_fail {
            no_free_slot,
            invalid_type,
            invalid_Capacity,
        };


        static void init();

        template <typename T,std::size_t Capacity>
        static acquire_token<T,Capacity> add_pool(std::size_t Count) {
            instance->pools.push_back(
              pool_data {
                .type_id = details::type_id<T>,
                .type_size = sizeof(T),
                .capacity = Capacity,
                .count = Count,
                .offset = 0
            });
            return acquire_token<T,Capacity> { .type_id = details::type_id<T>};
        }

        static bool alloc_pools();

        template <typename T,std::size_t Capacity>
        static std::expected<stream_buffer<T>,acquire_fail> acquire(acquire_token<T,Capacity> token);

        static void terminate();
        //add pool후에 각 pipeline stage에 적절한 token을 전달해주어서 stage가 CTAD로 type, capacity 파악하게 하는 것이
        //좋을듯.
        /*
            auto u16_token = add_pool<uint16_t, 10,10>
            ...

            rescaler {in: u8_token,out: u16_token}

            -> rescaler<typename In, typename Out> (In,Out)

            in rescaler
            In::type vec = ....
            In::capacity...

         */
    private:
        bool add_pool_impl(
            details::type_id_t type_id,
            std::size_t type_size,
            std::size_t N,
            std::size_t count
            );

        void* acquire_impl(
            details::type_id_t type_id,
            std::size_t type_size,
            std::size_t N
            );

        bool m_allocated = false;

        std::vector<pool_data> pools;
        std::vector<std::byte> data;

        inline static stream_pool_manager* instance = nullptr;
    };
}
#endif //DSP_STREAM_POOL_HPP
