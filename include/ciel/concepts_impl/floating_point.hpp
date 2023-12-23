#ifndef CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_FLOATING_POINT_HPP_
#define CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_FLOATING_POINT_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/is_floating_point.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
concept floating_point = is_floating_point_v<T>;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_FLOATING_POINT_HPP_