#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_LVALUE_REFERENCE_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_LVALUE_REFERENCE_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/integral_constant.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
struct is_lvalue_reference : false_type {};

template<class T>
struct is_lvalue_reference<T&> : true_type {};

template<class T>
inline constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_LVALUE_REFERENCE_HPP_