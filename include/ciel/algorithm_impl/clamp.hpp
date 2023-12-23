#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_CLAMP_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_CLAMP_HPP_

#include <ciel/config.hpp>
#include <ciel/functional_impl/less.hpp>

NAMESPACE_CIEL_BEGIN

template<class T, class Compare>
[[nodiscard]] constexpr auto clamp(const T& v, const T& lo, const T& hi, Compare comp) -> const T& {
    return comp(v, lo) ? lo : comp(hi, v) ? hi : v;
}

template<class T>
[[nodiscard]] constexpr auto clamp(const T& v, const T& lo, const T& hi) -> const T& {
    return ciel::clamp(v, lo, hi, less<>());
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_CLAMP_HPP_