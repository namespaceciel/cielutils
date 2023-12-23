#ifndef CIELUTILS_INCLUDE_CIEL_NUMERIC_IMPL_GCD_HPP_
#define CIELUTILS_INCLUDE_CIEL_NUMERIC_IMPL_GCD_HPP_

#include <ciel/concepts_impl/integral.hpp>
#include <ciel/config.hpp>
#include <ciel/type_traits_impl/common_type.hpp>
#include <ciel/type_traits_impl/is_same.hpp>

NAMESPACE_CIEL_BEGIN

template<integral M, integral N>
    requires (!is_same_v<remove_cv_t<M>, bool> && !is_same_v<remove_cv_t<N>, bool>)
constexpr auto gcd(M m, N n) -> common_type_t<M, N> {
    return n == 0 ? m : gcd(n, m % n);
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_NUMERIC_IMPL_GCD_HPP_