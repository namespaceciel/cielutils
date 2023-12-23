#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_PREV_PERMUTATION_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_PREV_PERMUTATION_HPP_

#include <ciel/algorithm_impl/reverse.hpp>
#include <ciel/config.hpp>
#include <ciel/functional_impl/less.hpp>

NAMESPACE_CIEL_BEGIN

template<class BidirIt, class Compare>
[[nodiscard]] constexpr auto prev_permutation(BidirIt first, BidirIt last, Compare comp) -> bool {
    if (first == last) {
        return false;
    }
    BidirIt i = last;
    if (first == --i) {
        return false;
    }

    while (true) {
        BidirIt i1 = i;
        BidirIt i2;

        if (comp(*i1, *--i)) {
            i2 = last;
            while (!comp(*--i2, *i)) {}
            ciel::iter_swap(i, i2);
            ciel::reverse(i1, last);
            return true;
        }
        if (i == first) {
            ciel::reverse(first, last);
            return false;
        }
    }
}

template<class BidirIt>
[[nodiscard]] constexpr auto prev_permutation(BidirIt first, BidirIt last) -> bool {
    return ciel::prev_permutation(first, last, less<>());
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_PREV_PERMUTATION_HPP_