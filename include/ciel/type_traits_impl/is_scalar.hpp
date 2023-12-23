#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_SCALAR_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_SCALAR_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/is_arithmetic.hpp>
#include <ciel/type_traits_impl/is_enum.hpp>
#include <ciel/type_traits_impl/is_member_pointer.hpp>
#include <ciel/type_traits_impl/is_null_pointer.hpp>
#include <ciel/type_traits_impl/is_pointer.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
struct is_scalar : bool_constant<is_arithmetic_v<T> || is_enum_v<T> || is_pointer_v<T> ||
                                 is_member_pointer_v<T> || is_null_pointer_v<T>> {};

template<class T>
inline constexpr bool is_scalar_v = is_scalar<T>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_SCALAR_HPP_