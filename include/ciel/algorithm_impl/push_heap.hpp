#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_PUSH_HEAP_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_PUSH_HEAP_HPP_

#include <ciel/config.hpp>
#include <ciel/functional_impl/less.hpp>
#include <ciel/iterator_impl/distance.hpp>

NAMESPACE_CIEL_BEGIN

// Note that the element to be pushed is already at index [last - 1]

template<class RandomIt, class Compare>
constexpr auto sift_up(RandomIt first, RandomIt last, Compare comp,
                       typename iterator_traits<RandomIt>::difference_type len) -> void {
    using value_type = typename iterator_traits<RandomIt>::value_type;
    if (len > 1) {
        len = (len - 2) / 2;
        RandomIt head = first + len;
        if (comp(*head, *--last)) {    // If head is less than itself, sift up
            value_type tmp_larger = std::move(*last);    // largest element so far, keep sifting up
            do {
                *last = std::move(*head);
                last = head;
                if (len == 0) {
                    break;
                }
                len = (len - 1) / 2;
                head = first + len;
            } while (comp(*head, tmp_larger));
            *last = std::move(tmp_larger);
        }
    }
}

namespace details {

template<class RandomIt, class Compare>
constexpr auto push_heap_helper(RandomIt first, RandomIt last, Compare comp) -> void {
    auto len = ciel::distance(first, last);
    ciel::sift_up(first, last, comp, len);
}

}   // namespace details

template<class RandomIt, class Compare>
constexpr auto push_heap(RandomIt first, RandomIt last, Compare comp) -> void {
    details::push_heap_helper(first, last, comp);
}

template<class RandomIt>
constexpr auto push_heap(RandomIt first, RandomIt last) -> void {
    details::push_heap_helper(first, last, less<>());
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_PUSH_HEAP_HPP_