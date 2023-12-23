#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_BINARY_SEARCH_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_BINARY_SEARCH_HPP_

#include <ciel/algorithm_impl/lower_bound.hpp>
#include <ciel/config.hpp>
#include <ciel/functional_impl/less.hpp>

NAMESPACE_CIEL_BEGIN

template<class ForwardIt, class T, class Compare>
[[nodiscard]] constexpr auto binary_search(ForwardIt first, ForwardIt last, const T& value, Compare comp) -> bool {
    first = ciel::lower_bound(first, last, value, comp);
    return first != last && !(comp(value, *first));
}

template<class ForwardIt, class T>
[[nodiscard]] constexpr auto binary_search(ForwardIt first, ForwardIt last, const T& value) -> bool {
    return ciel::binary_search(first, last, value, less<>());
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_BINARY_SEARCH_HPP_