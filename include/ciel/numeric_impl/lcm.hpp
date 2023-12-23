#ifndef CIELUTILS_INCLUDE_CIEL_NUMERIC_IMPL_LCM_HPP_
#define CIELUTILS_INCLUDE_CIEL_NUMERIC_IMPL_LCM_HPP_

#include <ciel/config.hpp>
#include <ciel/numeric_impl/gcd.hpp>

NAMESPACE_CIEL_BEGIN

template<class M, class N>
constexpr auto lcm(M m, N n) -> common_type_t<M, N> {
    return m / ciel::gcd(m, n) * n;
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_NUMERIC_IMPL_LCM_HPP_