#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_CONSTRUCTIBLE_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_CONSTRUCTIBLE_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/integral_constant.hpp>

NAMESPACE_CIEL_BEGIN

template<class T, class ...Args>
struct is_constructible : bool_constant<__is_constructible(T, Args...)> {};

template<class T, class ...Args>
struct is_trivially_constructible : bool_constant<__is_trivially_constructible(T, Args...)> {};

template<class T, class ...Args>
struct is_nothrow_constructible : bool_constant<__is_nothrow_constructible(T, Args...)> {};

template<class T, class ...Args>
inline constexpr bool is_constructible_v = is_constructible<T, Args...>::value;

template<class T, class ...Args>
inline constexpr bool is_trivially_constructible_v = is_trivially_constructible<T, Args...>::value;

template<class T, class ...Args>
inline constexpr bool is_nothrow_constructible_v = is_nothrow_constructible<T, Args...>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_CONSTRUCTIBLE_HPP_