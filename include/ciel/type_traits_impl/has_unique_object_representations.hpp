#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_HAS_UNIQUE_OBJECT_REPRESENTATIONS_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_HAS_UNIQUE_OBJECT_REPRESENTATIONS_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/integral_constant.hpp>
#include <ciel/type_traits_impl/remove_all_extents.hpp>
#include <ciel/type_traits_impl/remove_cv.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
struct has_unique_object_representations
    : bool_constant<__has_unique_object_representations(remove_cv_t<remove_all_extents_t<T>>)> {};

template<class T>
inline constexpr bool has_unique_object_representations_v = has_unique_object_representations<T>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_HAS_UNIQUE_OBJECT_REPRESENTATIONS_HPP_