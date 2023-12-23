#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_MAKE_HEAP_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_MAKE_HEAP_HPP_

#include <ciel/algorithm_impl/sift_down.hpp>
#include <ciel/config.hpp>
#include <ciel/functional_impl/less.hpp>
#include <ciel/iterator_impl/distance.hpp>

NAMESPACE_CIEL_BEGIN

namespace details {

template<class RandomIt, class Compare>
constexpr auto make_heap_helper(RandomIt first, RandomIt last, Compare comp) -> void {
    auto len = ciel::distance(first, last);
    if (len > 1) {
        for (auto offset = (len - 2) / 2; offset >= 0; --offset) {
            ciel::sift_down(first, comp, len, first + offset);
        }
    }
}

}   // namespace details

template<class RandomIt, class Compare>
constexpr auto make_heap(RandomIt first, RandomIt last, Compare comp) -> void {
    details::make_heap_helper(first, last, comp);
}

template<class RandomIt>
constexpr auto make_heap(RandomIt first, RandomIt last) -> void {
    details::make_heap_helper(first, last, less<>());
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_MAKE_HEAP_HPP_