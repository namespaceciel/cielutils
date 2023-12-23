#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_EXTENT_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_EXTENT_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/integral_constant.hpp>
#include <cstddef>

NAMESPACE_CIEL_BEGIN

template<class T, unsigned N = 0>
struct extent : integral_constant<size_t, 0> {};

template<class T>
struct extent<T[], 0> : integral_constant<size_t, 0> {};

template<class T, unsigned N>
struct extent<T[], N> : extent<T, N - 1> {};

template<class T, size_t I>
struct extent<T[I], 0> : integral_constant<size_t, I> {};

template<class T, size_t I, unsigned N>
struct extent<T[I], N> : extent<T, N - 1> {};

template<class T, unsigned N = 0>
inline constexpr size_t extent_v = extent<T, N>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_EXTENT_HPP_