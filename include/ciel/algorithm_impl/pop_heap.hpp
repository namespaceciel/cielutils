#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_POP_HEAP_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_POP_HEAP_HPP_

#include <ciel/algorithm_impl/sift_down.hpp>
#include <ciel/config.hpp>
#include <ciel/functional_impl/less.hpp>

NAMESPACE_CIEL_BEGIN

namespace details {

template<class RandomIt, class Compare>
constexpr auto pop_heap_helper(RandomIt first, RandomIt last, Compare comp) -> void {
    std::swap(*first, *(last - 1));
    ciel::sift_down(first, comp, last - first - 1, first);
}

}   // namespace details

template<class RandomIt, class Compare>
constexpr auto pop_heap(RandomIt first, RandomIt last, Compare comp) -> void {
    details::pop_heap_helper(first, last, comp);
}

template<class RandomIt>
constexpr auto pop_heap(RandomIt first, RandomIt last) -> void {
    details::pop_heap_helper(first, last, less<>());
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_POP_HEAP_HPP_