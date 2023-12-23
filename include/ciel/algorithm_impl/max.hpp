#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_MAX_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_MAX_HPP_

#include <ciel/algorithm_impl/max_element.hpp>
#include <ciel/config.hpp>
#include <ciel/functional_impl/less.hpp>
#include <initializer_list>

NAMESPACE_CIEL_BEGIN

template<class T, class Compare>
[[nodiscard]] constexpr auto max(const T& a, const T& b, Compare comp) -> const T& {
    return comp(a, b) ? b : a;
}

template<class T>
[[nodiscard]] constexpr auto max(const T& a, const T& b) -> const T& {
    return ciel::max(a, b, less<>());
}

template<class T, class Compare>
[[nodiscard]] constexpr auto max(std::initializer_list<T> ilist, Compare comp) -> T {
    return *ciel::max_element(ilist.begin(), ilist.end(), comp);
}

template<class T>
[[nodiscard]] constexpr auto max(std::initializer_list<T> ilist) -> T {
    return ciel::max(ilist, less<>());
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_MAX_HPP_