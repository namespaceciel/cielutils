#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_SWAP_RANGES_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_SWAP_RANGES_HPP_

#include <ciel/algorithm_impl/iter_swap.hpp>
#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

template<class ForwardIt1, class ForwardIt2>
constexpr auto swap_ranges(ForwardIt1 first1, ForwardIt1 last1, ForwardIt2 first2) -> ForwardIt2 {
    while (first1 != last1) {
        ciel::iter_swap(first1++, first2++);
    }
    return first2;
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_SWAP_RANGES_HPP_