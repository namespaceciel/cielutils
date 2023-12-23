#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_RANK_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_RANK_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/integral_constant.hpp>
#include <cstddef>

NAMESPACE_CIEL_BEGIN

template<class T>
struct rank : integral_constant<size_t, 0> {};

template<class T>
struct rank<T[]> : integral_constant<size_t, rank<T>::value + 1> {};

template<class T, size_t N>
struct rank<T[N]> : integral_constant<size_t, rank<T>::value + 1> {};

template<class T>
inline constexpr size_t rank_v = rank<T>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_RANK_HPP_