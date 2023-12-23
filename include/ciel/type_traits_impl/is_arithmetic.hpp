#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_ARITHMETIC_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_ARITHMETIC_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/is_floating_point.hpp>
#include <ciel/type_traits_impl/is_integral.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
struct is_arithmetic : bool_constant<is_integral_v<T> || is_floating_point_v<T> > {};

template<class T>
inline constexpr bool is_arithmetic_v = is_arithmetic<T>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_ARITHMETIC_HPP_