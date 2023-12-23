#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_HAS_VIRTUAL_DESTRUCTOR_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_HAS_VIRTUAL_DESTRUCTOR_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/integral_constant.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
struct has_virtual_destructor : bool_constant<__has_virtual_destructor(T)> {};

template<class T>
inline constexpr bool has_virtual_destructor_v = has_virtual_destructor<T>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_HAS_VIRTUAL_DESTRUCTOR_HPP_