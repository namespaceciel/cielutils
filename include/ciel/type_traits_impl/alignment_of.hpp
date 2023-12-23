#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_ALIGNMENT_OF_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_ALIGNMENT_OF_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/integral_constant.hpp>
#include <cstddef>

NAMESPACE_CIEL_BEGIN

template<class T>
struct alignment_of : integral_constant<size_t, alignof(T)> {};

template<class T>
inline constexpr size_t alignment_of_v = alignment_of<T>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_ALIGNMENT_OF_HPP_