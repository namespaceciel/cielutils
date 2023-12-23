#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_VOID_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_VOID_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/is_same.hpp>
#include <ciel/type_traits_impl/remove_cv.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
struct is_void : is_same<void, remove_cv_t<T>> {};

template<class T>
inline constexpr bool is_void_v = is_void<T>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_VOID_HPP_