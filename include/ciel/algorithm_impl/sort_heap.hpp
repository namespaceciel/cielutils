#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_SORT_HEAP_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_SORT_HEAP_HPP_

#include <ciel/algorithm_impl/pop_heap.hpp>
#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

namespace details {

template<class RandomIt, class Compare>
constexpr auto sort_heap_helper(RandomIt first, RandomIt last, Compare&& comp) -> void {
    while (last != first) {
        ciel::pop_heap(first, last, comp);
        --last;
    }
}

}   // namespace details

template<class RandomIt, class Compare>
constexpr auto sort_heap(RandomIt first, RandomIt last, Compare comp) -> void {
    details::sort_heap_helper(first, last, comp);
}

template<class RandomIt>
constexpr auto sort_heap(RandomIt first, RandomIt last) -> void {
    details::sort_heap_helper(first, last, less<>());
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_SORT_HEAP_HPP_