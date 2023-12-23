#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_NULL_POINTER_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_NULL_POINTER_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/is_same.hpp>
#include <ciel/type_traits_impl/remove_cv.hpp>
#include <cstddef>

NAMESPACE_CIEL_BEGIN

template<class T>
struct is_null_pointer : is_same<std::nullptr_t, remove_cv_t<T>> {};

template<class T>
inline constexpr bool is_null_pointer_v = is_null_pointer<T>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_NULL_POINTER_HPP_