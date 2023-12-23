#ifndef CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_SIGNED_INTEGRAL_HPP_
#define CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_SIGNED_INTEGRAL_HPP_

#include <ciel/concepts_impl/integral.hpp>
#include <ciel/config.hpp>
#include <ciel/type_traits_impl/is_signed.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
concept signed_integral = integral<T> && is_signed_v<T>;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_SIGNED_INTEGRAL_HPP_