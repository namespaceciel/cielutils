#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_COPY_CONSTRUCTIBLE_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_COPY_CONSTRUCTIBLE_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/add_cv.hpp>
#include <ciel/type_traits_impl/add_lvalue_reference.hpp>
#include <ciel/type_traits_impl/is_constructible.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
struct is_copy_constructible : is_constructible<T, add_lvalue_reference_t<add_const_t<T>>> {};

template<class T>
struct is_trivially_copy_constructible : is_trivially_constructible<T, add_lvalue_reference_t<add_const_t<T>>> {};

template<class T>
struct is_nothrow_copy_constructible : is_nothrow_constructible<T, add_lvalue_reference_t<add_const_t<T>>> {};

template<class T>
inline constexpr bool is_copy_constructible_v = is_copy_constructible<T>::value;

template<class T>
inline constexpr bool is_trivially_copy_constructible_v = is_trivially_copy_constructible<T>::value;

template<class T>
inline constexpr bool is_nothrow_copy_constructible_v = is_nothrow_copy_constructible<T>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_COPY_CONSTRUCTIBLE_HPP_