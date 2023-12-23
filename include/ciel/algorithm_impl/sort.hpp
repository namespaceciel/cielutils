#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_SORT_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_SORT_HPP_

#include <bit>  // for std::bit_width
#include <ciel/algorithm_impl/move_backward.hpp>
#include <ciel/algorithm_impl/partial_sort.hpp>
#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

// Introsort is consist of quicksort, heapsort and insertionsort.
// We use bid_width to calculate log(N) (N is the range size), which is the ideal depth in quicksort,
// if depth is beyond 2 * log(N), we switch to heapsort.
//
// When N <= Threshold, we switch to insertionsort, because insertionsort can get good performance
// when dealing with nearly ordered range. We have already cut N-size range into many small ranges,
// and these ranges are ordered to each other, we insertionsort first range,
// and use unguarded_linear_insert on following ranges to get rid of bound checking.

namespace details {

constexpr int Threshold = 16;

template<class RandomIt, class Compare>
constexpr auto unguarded_linear_insert(RandomIt last, Compare comp) -> void {
    typename iterator_traits<RandomIt>::value_type val = std::move(*last);
    RandomIt next = last - 1;
    while (comp(val, *next)) {
        *last = std::move(*next);
        last = next;
        --next;
    }
    *last = std::move(val);
}

template<class RandomIt, class Compare>
constexpr auto insertion_sort(RandomIt first, RandomIt last, Compare comp) -> void {
    if (first == last) {
        return;
    }
    for (RandomIt it = first + 1; it != last; ++it) {
        if (comp(*it, *first)) {
            typename iterator_traits<RandomIt>::value_type val = std::move(*it);
            std::move_backward(first, it, it + 1);
            *first = std::move(val);
        } else {
            unguarded_linear_insert(it, comp);
        }
    }
}

template<class RandomIt, class Compare>
constexpr auto unguarded_insertion_sort(RandomIt first, RandomIt last, Compare comp) -> void {
    while (first != last) {
        unguarded_linear_insert(first, comp);
        ++first;
    }
}

template<class RandomIt, class Compare>
constexpr auto final_insertion_sort(RandomIt first, RandomIt last, Compare comp) -> void {
    if (last - first > Threshold) {
        insertion_sort(first, first + Threshold, comp);
        unguarded_insertion_sort(first + Threshold, last, comp);
    } else {
        insertion_sort(first, last, comp);
    }
}

template<class RandomIt, class Compare>
constexpr auto move_median_to_first(RandomIt first, RandomIt mid, RandomIt last, Compare comp) -> void {
    if (comp(*first, *mid)) {
        if (comp(*mid, *last)) {
            ciel::iter_swap(first, mid);
        } else if (comp(*first, *last)) {
            ciel::iter_swap(first, last);
        }
    } else if (comp(*first, *last)) {
        // return;
    } else if (comp(*mid, *last)) {
        ciel::iter_swap(first, last);
    } else {
        ciel::iter_swap(first, mid);
    }
}

template<class RandomIt, class Compare>
constexpr auto partition(RandomIt first, RandomIt last, RandomIt pivot, Compare comp) -> RandomIt {
    while (true) {
        while (comp(*first, *pivot)) {
            ++first;
        }
        --last;
        while (comp(*pivot, *last)) {
            --last;
        }
        if (first >= last) {
            return first;
        }
        ciel::iter_swap(first, last);
        ++first;
    }
}

// This range should have at least one element
template<class RandomIt, class Compare>
constexpr auto partition_pivot(RandomIt first, RandomIt last, Compare comp) -> RandomIt {
    RandomIt mid = first + (last - first) / 2;
    move_median_to_first(first, mid, last - 1, comp);
    return partition(first + 1, last, first, comp);
}

template<class RandomIt, class Compare>
constexpr auto introsort_loop(RandomIt first, RandomIt last, size_t depth_limit, Compare comp) -> void {
    while (last - first > Threshold) {
        if (depth_limit == 0) {
            ciel::partial_sort(first, last, last, comp);
            return;
        }
        --depth_limit;
        RandomIt cut = partition_pivot(first, last, comp);
        introsort_loop(cut + 1, last, depth_limit, comp);
        last = cut;
    }
}

}   // namespace details

template<class RandomIt, class Compare>
constexpr auto sort(RandomIt first, RandomIt last, Compare comp) -> void {
    if (first == last) {
        return;
    }
    details::introsort_loop(first, last, 2 * std::bit_width(static_cast<size_t>(last - first)), comp);
    details::final_insertion_sort(first, last, comp);
}

template<class RandomIt>
constexpr auto sort(RandomIt first, RandomIt last) -> void {
    ciel::sort(first, last, less<>());
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_SORT_HPP_