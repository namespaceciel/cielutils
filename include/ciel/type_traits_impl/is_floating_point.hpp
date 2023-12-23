#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_FLOATING_POINT_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_FLOATING_POINT_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/is_same.hpp>
#include <ciel/type_traits_impl/remove_cv.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
struct is_floating_point : bool_constant<is_same_v<float, remove_cv_t<T>>
                                            || is_same_v<double, remove_cv_t<T>>
                                            || is_same_v<long double, remove_cv_t<T>>> {};

template<class T>
inline constexpr bool is_floating_point_v = is_floating_point<T>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_FLOATING_POINT_HPP_