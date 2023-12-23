#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_FIND_INDEX_IN_ARGS_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_FIND_INDEX_IN_ARGS_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/is_same.hpp>

NAMESPACE_CIEL_BEGIN

// serve for get in tuple and variant

template<size_t I, class T, class Front, class... Rest>
struct find_index_in_Args : find_index_in_Args<I + 1, T, Rest...> {};

template<size_t I, class T, class... Rest>
struct find_index_in_Args<I, T, T, Rest...> : integral_constant<size_t, I> {
    static_assert(!(is_same_v<T, Rest> || ...), "T shows more than one time in Args.");
};

template<size_t I, class T, class... Args>
inline static constexpr size_t find_index_in_Args_v = find_index_in_Args<I, T, Args...>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_FIND_INDEX_IN_ARGS_HPP_