#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_ASSIGNABLE_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_ASSIGNABLE_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/integral_constant.hpp>

NAMESPACE_CIEL_BEGIN

template<class T, class U>
struct is_assignable : bool_constant<__is_assignable(T, U)> {};

template<class T, class U>
struct is_trivially_assignable : bool_constant<__is_trivially_assignable(T, U)> {};

template<class T, class U>
struct is_nothrow_assignable : bool_constant<__is_nothrow_assignable(T, U)> {};

template<class T, class U>
inline constexpr bool is_assignable_v = is_assignable<T, U>::value;

template<class T, class U>
inline constexpr bool is_trivially_assignable_v = is_trivially_assignable<T, U>::value;

template<class T, class U>
inline constexpr bool is_nothrow_assignable_v = is_nothrow_assignable<T, U>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_ASSIGNABLE_HPP_