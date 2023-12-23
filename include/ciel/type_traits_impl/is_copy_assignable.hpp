#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_COPY_ASSIGNABLE_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_COPY_ASSIGNABLE_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/add_lvalue_reference.hpp>
#include <ciel/type_traits_impl/is_assignable.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
struct is_copy_assignable
    : is_assignable<add_lvalue_reference_t<T>, add_lvalue_reference_t<const T>> {};

template<class T>
struct is_trivially_copy_assignable
    : is_trivially_assignable<add_lvalue_reference_t<T>, add_lvalue_reference_t<const T>> {};

template<class T>
struct is_nothrow_copy_assignable
    : is_nothrow_assignable<add_lvalue_reference_t<T>, add_lvalue_reference_t<const T>> {};

template<class T>
inline constexpr bool is_copy_assignable_v = is_copy_assignable<T>::value;

template<class T>
inline constexpr bool is_trivially_copy_assignable_v = is_trivially_copy_assignable<T>::value;

template<class T>
inline constexpr bool is_nothrow_copy_assignable_v = is_nothrow_copy_assignable<T>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_COPY_ASSIGNABLE_HPP_