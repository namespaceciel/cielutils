#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_MIN_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_MIN_HPP_

#include <ciel/algorithm_impl/min_element.hpp>
#include <ciel/config.hpp>
#include <ciel/functional_impl/less.hpp>
#include <initializer_list>

NAMESPACE_CIEL_BEGIN

template<class T, class Compare>
[[nodiscard]] constexpr auto min(const T& a, const T& b, Compare comp) -> const T& {
    return comp(b, a) ? b : a;
}

template<class T>
[[nodiscard]] constexpr auto min(const T& a, const T& b) -> const T& {
    return ciel::min(a, b, less<>());
}

template<class T, class Compare>
[[nodiscard]] constexpr auto min(std::initializer_list<T> ilist, Compare comp) -> T {
    return *ciel::min_element(ilist.begin(), ilist.end(), comp);
}

template<class T>
[[nodiscard]] constexpr auto min(std::initializer_list<T> ilist) -> T {
    return ciel::min(ilist, less<>());
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_MIN_HPP_