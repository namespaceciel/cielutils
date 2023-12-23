#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_DEFAULT_CONSTRUCTIBLE_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_DEFAULT_CONSTRUCTIBLE_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/is_constructible.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
struct is_default_constructible : is_constructible<T> {};

template<class T>
struct is_trivially_default_constructible : is_trivially_constructible<T> {};

template<class T>
struct is_nothrow_default_constructible : is_nothrow_constructible<T> {};

template<class T>
inline constexpr bool is_default_constructible_v = is_default_constructible<T>::value;

template<class T>
inline constexpr bool is_trivially_default_constructible_v = is_trivially_default_constructible<T>::value;

template<class T>
inline constexpr bool is_nothrow_default_constructible_v = is_nothrow_default_constructible<T>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_DEFAULT_CONSTRUCTIBLE_HPP_