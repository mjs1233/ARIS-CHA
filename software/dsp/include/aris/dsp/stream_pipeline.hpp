//
// Created by takakura on 8/30/26.
//

#ifndef DSP_STREAM_PIPELINE_HPP
#define DSP_STREAM_PIPELINE_HPP
#include "function_traits.hpp"
#include <tuple>

#include "stream_block.hpp"

#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace aris::dsp {

    template <typename T>
    struct task_holder {
        using traits = callable_traits<T>;
        using result = typename traits::return_type;

        T task;

        template <typename DataType, typename ContextType>
        void exec(stream_block<DataType>& block, ContextType& context) {
            std::invoke(task,block, context);
        }
    };

    template <typename... Ts>
    struct stream_pipeline {
        std::tuple<task_holder<Ts>...> tasks;

        template <typename DataType, typename ContextType>
        void exec(stream_block<DataType>& block, ContextType& context) {
            std::apply(
                [&](auto&... tasks) {
                    (tasks.exec(block, context), ...);
                },
                tasks
            );
        }
    };

    template <typename... Ts, typename Task>
    auto operator|(
        stream_pipeline<Ts...> pipeline,
        Task&& task
    ) {
        using task_type = std::decay_t<Task>;
        using holder_type = task_holder<task_type>;
        using pipeline_type =
            stream_pipeline<Ts..., task_type>;

        return std::apply(
            [&](auto&&... tasks) {
                return pipeline_type{
                    std::tuple{
                        std::forward<decltype(tasks)>(tasks)...,
                        holder_type{
                            std::forward<Task>(task)
                        }
                    }
                };
            },
            std::move(pipeline.tasks)
        );
    }

}

#endif //DSP_STREAM_PIPELINE_HPP
