#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_FUNDAMENTAL_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_FUNDAMENTAL_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/is_arithmetic.hpp>
#include <ciel/type_traits_impl/is_null_pointer.hpp>
#include <ciel/type_traits_impl/is_void.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
struct is_fundamental : bool_constant<is_arithmetic_v<T> || is_void_v<T> || is_null_pointer_v<T>> {};

template<class T>
inline constexpr bool is_fundamental_v = is_fundamental<T>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_FUNDAMENTAL_HPP_