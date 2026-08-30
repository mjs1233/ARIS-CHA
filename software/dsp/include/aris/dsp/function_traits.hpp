//
// Created by takakura on 8/30/26.
//

#ifndef DSP_FUNCTION_TRAIT_HPP
#define DSP_FUNCTION_TRAIT_HPP
#include "meta/type_list.hpp"
namespace aris::dsp {

    template <typename T>
    struct function_traits;

    template <typename R, typename C, typename... Args>
    struct function_traits<R(C::*)(Args...)> {
        using return_type = R;
        using args_list  = meta::type_list<std::remove_cvref_t<Args>...>;

        static constexpr std::size_t arity = sizeof...(Args);

        template <std::size_t I>
        using arg_t = meta::at<args_list, I>;
    };

    template <typename R, typename C, typename... Args>
    struct function_traits<R(C::*)(Args...) const> {
        using return_type = R;
        using args_list  = meta::type_list<std::remove_cvref_t<Args>...>;

        static constexpr std::size_t arity = sizeof...(Args);

        template <std::size_t I>
        using arg_t = meta::at<args_list, I>;
    };

    template <typename R, typename C, typename... Args>
    struct function_traits<R(C::*)(Args...) const noexcept>
    : function_traits<R(C::*)(Args...) const> {};

    template <typename R, typename C, typename... Args>
    struct function_traits<R(C::*)(Args...) noexcept>
        : function_traits<R(C::*)(Args...)> {};

    template <typename F>
    struct callable_traits
    : function_traits<
        decltype(&std::remove_cvref_t<F>::operator())
    > {};

}
#endif //DSP_FUNCTION_TRAIT_HPP
