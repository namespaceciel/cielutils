#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_TRIVIAL_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_TRIVIAL_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/is_default_constructible.hpp>
#include <ciel/type_traits_impl/is_trivially_copyable.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
struct is_trivial : bool_constant<is_trivially_copyable_v<T> && is_trivially_default_constructible_v<T>> {};

template<class T>
inline constexpr bool is_trivial_v = is_trivial<T>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_TRIVIAL_HPP_