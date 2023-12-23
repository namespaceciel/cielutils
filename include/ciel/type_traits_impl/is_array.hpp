#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_ARRAY_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_ARRAY_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/integral_constant.hpp>
#include <cstddef>

NAMESPACE_CIEL_BEGIN

template<class T>
struct is_array : false_type {};

template<class T>
struct is_array<T[]> : true_type {};

template<class T, size_t N>
struct is_array<T[N]> : true_type {};

template<class T>
inline constexpr bool is_array_v = is_array<T>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_ARRAY_HPP_