#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_TRIVIALLY_COPYABLE_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_TRIVIALLY_COPYABLE_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/integral_constant.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
struct is_trivially_copyable : bool_constant<__is_trivially_copyable(T)> {};

template<class T>
inline constexpr bool is_trivially_copyable_v = is_trivially_copyable<T>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_TRIVIALLY_COPYABLE_HPP_