#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_EQUAL_RANGE_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_EQUAL_RANGE_HPP_

#include <ciel/algorithm_impl/lower_bound.hpp>
#include <ciel/algorithm_impl/upper_bound.hpp>
#include <ciel/config.hpp>
#include <ciel/functional_impl/less.hpp>
#include <ciel/utility_impl/pair.hpp>

NAMESPACE_CIEL_BEGIN

template<class ForwardIt, class T, class Compare>
[[nodiscard]] constexpr auto equal_range(ForwardIt first, ForwardIt last, const T& value, Compare comp)
    -> pair<ForwardIt, ForwardIt> {
    return {ciel::lower_bound(first, last, value, comp), ciel::upper_bound(first, last, value, comp)};
}

template<class ForwardIt, class T>
[[nodiscard]] constexpr auto equal_range(ForwardIt first, ForwardIt last, const T& value)
    -> pair<ForwardIt, ForwardIt> {
    return ciel::equal_range(first, last, value, less<>());
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_EQUAL_RANGE_HPP_