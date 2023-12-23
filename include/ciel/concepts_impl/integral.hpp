#ifndef CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_INTEGRAL_HPP_
#define CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_INTEGRAL_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/is_integral.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
concept integral = is_integral_v<T>;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_INTEGRAL_HPP_