#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_SAME_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_SAME_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/integral_constant.hpp>

NAMESPACE_CIEL_BEGIN

template<class, class>
struct is_same : false_type {};

template<class T>
struct is_same<T, T> : true_type {};

template<class T, class U>
inline constexpr bool is_same_v = is_same<T, U>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_SAME_HPP_