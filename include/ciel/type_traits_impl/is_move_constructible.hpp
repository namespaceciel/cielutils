#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_MOVE_CONSTRUCTIBLE_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_MOVE_CONSTRUCTIBLE_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/add_rvalue_reference.hpp>
#include <ciel/type_traits_impl/is_constructible.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
struct is_move_constructible : is_constructible<T, add_rvalue_reference_t<T>> {};

template<class T>
struct is_trivially_move_constructible : is_trivially_constructible<T, add_rvalue_reference_t<T>> {};

template<class T>
struct is_nothrow_move_constructible : is_nothrow_constructible<T, add_rvalue_reference_t<T>> {};

template<class T>
inline constexpr bool is_move_constructible_v = is_move_constructible<T>::value;

template<class T>
inline constexpr bool is_trivially_move_constructible_v = is_trivially_move_constructible<T>::value;

template<class T>
inline constexpr bool is_nothrow_move_constructible_v = is_nothrow_move_constructible<T>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_MOVE_CONSTRUCTIBLE_HPP_