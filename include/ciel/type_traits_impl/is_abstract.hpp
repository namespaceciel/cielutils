#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_ABSTRACT_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_ABSTRACT_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/integral_constant.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
struct is_abstract : bool_constant<__is_abstract(T)> {};

template<class T>
inline constexpr bool is_abstract_v = is_abstract<T>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_ABSTRACT_HPP_