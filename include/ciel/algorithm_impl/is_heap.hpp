#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_IS_HEAP_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_IS_HEAP_HPP_

#include <ciel/algorithm_impl/is_heap_until.hpp>
#include <ciel/config.hpp>
#include <ciel/functional_impl/less.hpp>

NAMESPACE_CIEL_BEGIN

template<class RandomIt, class Compare>
[[nodiscard]] constexpr auto is_heap(RandomIt first, RandomIt last, Compare comp) -> bool {
    return details::is_heap_until_helper(first, last, comp) == last;
}

template<class RandomIt>
[[nodiscard]] constexpr auto is_heap(RandomIt first, RandomIt last) -> bool {
    return details::is_heap_until_helper(first, last, less<>()) == last;
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_IS_HEAP_HPP_