#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_PARTIAL_SORT_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_PARTIAL_SORT_HPP_

#include <ciel/algorithm_impl/iter_swap.hpp>
#include <ciel/algorithm_impl/make_heap.hpp>
#include <ciel/algorithm_impl/push_heap.hpp>
#include <ciel/algorithm_impl/sort_heap.hpp>
#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

template<class RandomIt, class Compare>
constexpr auto partial_sort(RandomIt first, RandomIt middle, RandomIt last, Compare comp) -> void {
    ciel::make_heap(first, middle, comp);
    for (RandomIt it = middle; it != last; ++it) {
        if (!comp(*first, *it)) {
            ciel::pop_heap(first, middle, comp);
            ciel::iter_swap(middle, it);
            ciel::push_heap(first, middle, comp);
        }
    }
    ciel::sort_heap(first, middle, comp);
}

template<class RandomIt>
constexpr auto partial_sort(RandomIt first, RandomIt middle, RandomIt last) -> void {
    ciel::partial_sort(first, middle, last, less<>());
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_PARTIAL_SORT_HPP_