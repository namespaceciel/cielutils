#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_MEMBER_OBJECT_POINTER_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_MEMBER_OBJECT_POINTER_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/is_member_function_pointer.hpp>
#include <ciel/type_traits_impl/is_member_pointer.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
struct is_member_object_pointer : bool_constant<is_member_pointer_v<T> && !is_member_function_pointer_v<T>> {};

template<class T>
inline constexpr bool is_member_object_pointer_v = is_member_object_pointer<T>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_MEMBER_OBJECT_POINTER_HPP_