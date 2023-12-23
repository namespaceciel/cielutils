#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_FUNCTION_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_FUNCTION_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/is_const.hpp>
#include <ciel/type_traits_impl/is_reference.hpp>

NAMESPACE_CIEL_BEGIN

// function and reference are always cv-qualified
template<class T>
struct is_function : bool_constant<!is_const_v<const T> && !is_reference_v<T>> {};

template<class T>
inline constexpr bool is_function_v = is_function<T>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_FUNCTION_HPP_