#ifndef CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_UNSIGNED_INTEGRAL_HPP_
#define CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_UNSIGNED_INTEGRAL_HPP_

#include <ciel/concepts_impl/signed_integral.hpp>
#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
concept unsigned_integral = integral<T> && !signed_integral<T>;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_UNSIGNED_INTEGRAL_HPP_