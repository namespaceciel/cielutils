#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_OBJECT_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_OBJECT_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/is_function.hpp>
#include <ciel/type_traits_impl/is_reference.hpp>
#include <ciel/type_traits_impl/is_void.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
struct is_object : bool_constant<!is_function_v<T> && !is_reference_v<T> && !is_void_v<T>> {};

template<class T>
inline constexpr bool is_object_v = is_object<T>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_OBJECT_HPP_