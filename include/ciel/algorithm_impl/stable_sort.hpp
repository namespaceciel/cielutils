#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_STABLE_SORT_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_STABLE_SORT_HPP_

#include <ciel/algorithm_impl/min.hpp>
#include <ciel/algorithm_impl/move.hpp>
#include <ciel/algorithm_impl/move_backward.hpp>
#include <ciel/algorithm_impl/reverse.hpp>
#include <ciel/algorithm_impl/upper_bound.hpp>
#include <ciel/config.hpp>
#include <ciel/iterator_impl/next.hpp>
#include <ciel/vector.hpp>

NAMESPACE_CIEL_BEGIN

// https://android.googlesource.com/platform/libcore/+/gingerbread/luni/src/main/java/java/util/TimSort.java

namespace details {

constexpr int MinMerge = 32;

template<class RandomIt>
struct run {
    RandomIt first_;
    typename iterator_traits<RandomIt>::difference_type len_;
};

// Find an increasing or decreasing range as long as possible (reverse it if it's decreasing)
// return the range size
// precondition: not empty range
template<class RandomIt, class Compare>
[[nodiscard]] constexpr auto count_run_and_make_ascending(RandomIt first, RandomIt last, Compare comp) -> typename iterator_traits<RandomIt>::difference_type {
    RandomIt it = ciel::next(first);
    if (it == last) {   // single element range
        return 1;
    }
    if (comp(*it, *first)) {    // decreasing
        do {
            ++it;
        } while (it != last && comp(*it, *(it - 1)));
        ciel::reverse(first, it);
    } else {    // increasing
        do {
            ++it;
        } while (it != last && !comp(*it, *(it - 1)));
    }
    return it - first;
}

// [first, sorted_last) is the increasing range from count_run_and_make_ascending()
// binary_insertion_sort the right half
// precondition: not empty range, sorted_last > first
template<class RandomIt, class Compare>
constexpr auto binary_insertion_sort(RandomIt first, RandomIt last, RandomIt sorted_last, Compare comp) -> void {
    for (; sorted_last != last; ++sorted_last) {
        typename iterator_traits<RandomIt>::value_type val = std::move(*sorted_last);
        auto pos = ciel::upper_bound(first, sorted_last, val, comp);
        std::move_backward(pos, sorted_last, sorted_last + 1);
        *pos = std::move(val);
    }
}

// check listsort.txt to see details
// precondition: n >= 0
template<class Size>
[[nodiscard]] constexpr auto min_run_length(Size n) -> Size {
    Size r = 0;
    while (n >= MinMerge) {
        r |= (n & 1);   //  r = r ? r : n % 2;
        n >>= 1;        //  n /= 2;
    }
    return n + r;
}

// TODO: The following ranges are always adjacent, which makes last1 === first2

template<class RandomIt, class Compare>
constexpr auto gallop_left(RandomIt first1, RandomIt last1, RandomIt first2, Compare comp) -> RandomIt {
    while (first1 != last1) {
        if (!comp(*first2, *first1)) {
            ++first1;
        } else {
            break;
        }
    }
    return first1;
}
template<class RandomIt, class Compare>
constexpr auto gallop_right(RandomIt last1, RandomIt first2, RandomIt last2, Compare comp) -> RandomIt {
    --last1;
    while (first2 != last2) {
        if (!comp(*(last2 - 1), *last1)) {
            --last2;
        } else {
            break;
        }
    }
    return last2;
}

// precondition: left half is shorter
template<class RandomIt, class Compare>
constexpr auto merge_low(RandomIt first1, RandomIt last1, RandomIt first2, RandomIt last2, Compare comp) -> void {
    using value_type = typename iterator_traits<RandomIt>::value_type;
    // store left half elements to tmp
    vector<value_type> tmp;
    tmp.reserve(last1 - first1);
    auto it = first1;
    while (it != last1) {
        tmp.push_back(std::move(*it));
        ++it;
    }

    auto b = tmp.begin();
    for (; b != tmp.end(); ++first1) {
        if (first2 == last2) {
            std::move(b, tmp.end(), first1);
            return;
        }
        if (comp(*first2, *b)) {
            *first1 = std::move(*first2);
            ++first2;
        } else {
            *first1 = std::move(*b);
            ++b;
        }
    }
    std::move(first2, last2, first1);
}

// precondition: right half is shorter
template<class RandomIt, class Compare>
constexpr auto merge_high(RandomIt first1, RandomIt last1, RandomIt first2, RandomIt last2, Compare comp) -> void {
    using value_type = typename iterator_traits<RandomIt>::value_type;
    // store right half elements to tmp
    vector<value_type> tmp;
    tmp.reserve(last2 - first2);
    auto it = first2;
    while (it != last2) {
        tmp.push_back(std::move(*it));
        ++it;
    }

    auto e = tmp.end();
    while (e != tmp.begin()) {
        if (first1 == last1) {
            std::move_backward(tmp.begin(), e, last2);
            return;
        }
        --last2;
        if (comp(*(e - 1), *(last1 - 1))) {
            --last1;
            *last2 = std::move(*last1);
        } else {
            --e;
            *last2 = std::move(*e);
        }
    }
    std::move_backward(first1, last1, last2);
}

template<class RandomIt, class Compare>
constexpr auto merge_at(vector<run<RandomIt>>& pending, typename vector<run<RandomIt>>::size_type n, Compare comp) -> void {
    RandomIt first1 = pending[n].first_;
    RandomIt last1 = first1 + pending[n].len_;
    RandomIt first2 = pending[n + 1].first_;
    RandomIt last2 = first2 + pending[n + 1].len_;

    first1 = details::gallop_left(first1, last1, first2, comp);
    last2 = details::gallop_right(last1, first2, last2, comp);

    if (last1 - first1 <= last2 - first2) {
        details::merge_low(first1, last1, first2, last2, comp);
    } else {
        details::merge_high(first1, last1, first2, last2, comp);
    }

    pending[n].len_ += pending[n + 1].len_;
    if (n == pending.size() - 3) {
        pending[n + 1] = pending[n + 2];
    }
    pending.pop_back();
}

}   // namespace details

template<class RandomIt, class Compare>
constexpr auto stable_sort(RandomIt first, RandomIt last, Compare comp) -> void {
    auto remaining_len = last - first;
    if (remaining_len < 2) {
        return;
    }
    // straightly insertionsort when range is too short
    if (remaining_len < details::MinMerge) {
        auto sorted_len = details::count_run_and_make_ascending(first, last, comp);
        details::binary_insertion_sort(first, last, first + sorted_len, comp);
        return;
    }

    // store every run (the increasing ranges to be merged)
    vector<details::run<RandomIt>> pending;

    auto min_run_len = details::min_run_length(remaining_len);

    RandomIt it = first;
    // remaining_len === last - it
    do {
        auto sorted_len = details::count_run_and_make_ascending(it, last, comp);
        auto forced_run_len = ciel::min(remaining_len, min_run_len);
        if (sorted_len < forced_run_len) {      // range size is too short
            details::binary_insertion_sort(it, it + forced_run_len, it + sorted_len, comp);
            sorted_len = forced_run_len;
        }

        pending.emplace_back(it, sorted_len);
        while (pending.size() > 1) {
            auto n = pending.size() - 2;

            // loop merging when meet one of these conditions:
            // 1. runLen[i - 3] > runLen[i - 2] + runLen[i - 1]
            // 2. runLen[i - 2] > runLen[i - 1]

            // merge the runs at n and n + 1
            if (n > 0 && pending[n - 1].len_ <= pending[n].len_ + pending[n + 1].len_) {
                if (pending[n - 1].len_ < pending[n + 1].len_) {
                    --n;
                }
                details::merge_at<RandomIt>(pending, n, comp);
            } else if (pending[n].len_ <= pending[n + 1].len_) {
                details::merge_at<RandomIt>(pending, n, comp);
            } else {
                break;
            }
        }

        it += sorted_len;
        remaining_len -= sorted_len;
    } while (remaining_len > 0);

    while (pending.size() > 1) {
        int n = pending.size() - 2;
        if (n > 0 && pending[n - 1].len_ < pending[n + 1].len_) {
            --n;
        }
        details::merge_at<RandomIt>(pending, n, comp);
    }
}

template<class RandomIt>
constexpr auto stable_sort(RandomIt first, RandomIt last) -> void {
    ciel::stable_sort(first, last, less<>());
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_STABLE_SORT_HPP_