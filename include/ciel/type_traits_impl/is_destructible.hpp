#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_DESTRUCTIBLE_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_DESTRUCTIBLE_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/conditional.hpp>
#include <ciel/type_traits_impl/is_function.hpp>
#include <ciel/type_traits_impl/is_reference.hpp>
#include <ciel/type_traits_impl/is_unbounded_array.hpp>
#include <ciel/type_traits_impl/is_void.hpp>
#include <ciel/type_traits_impl/remove_all_extents.hpp>
#include <ciel/utility_impl/declval.hpp>

NAMESPACE_CIEL_BEGIN

namespace details {

// https://stackoverflow.com/questions/54699526/why-is-is-destructible-defined-using-declvalu-u-and-not-declvalu
template<class T>
concept has_dtor = requires { declval<T&>().~T(); };

template<class T>
concept has_noexcept_dtor = requires {{ declval<T&>().~T() } noexcept; };

}   // namespace details

template<class T>
struct is_destructible
    : conditional_t<is_reference_v<T>,
                          true_type,
                          conditional_t<is_void_v<T> || is_function_v<T> || is_unbounded_array_v<T>,
                                              false_type,
                                              conditional_t<details::has_dtor<remove_all_extents_t<T>>,
                                                                  true_type,
                                                                  false_type>>> {};

// gcc doesn't have __is_trivially_destructible
#if __has_builtin(__is_trivially_destructible)

template<class T>
struct is_trivially_destructible : bool_constant<__is_trivially_destructible(T)> {};

#else

template<class T>
struct is_trivially_destructible : bool_constant<is_destructible<T>::value && __has_trivial_destructor(T)> {};

#endif

template<class T>
struct is_trivially_destructible<T[]> : false_type {};

template<class T>
struct is_nothrow_destructible
    : conditional_t<is_reference_v<T>,
                          true_type,
                          conditional_t<is_void_v<T> || is_function_v<T> || is_unbounded_array_v<T>,
                                              false_type,
                                              conditional_t<details::has_noexcept_dtor<remove_all_extents_t<T>>,
                                                                  true_type,
                                                                  false_type>>> {};

template<class T>
inline constexpr bool is_destructible_v = is_destructible<T>::value;

template<class T>
inline constexpr bool is_trivially_destructible_v = is_trivially_destructible<T>::value;

template<class T>
inline constexpr bool is_nothrow_destructible_v = is_nothrow_destructible<T>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_DESTRUCTIBLE_HPP_