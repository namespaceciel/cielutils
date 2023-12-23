#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_UPPER_BOUND_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_UPPER_BOUND_HPP_

#include <ciel/config.hpp>
#include <ciel/functional_impl/less.hpp>
#include <ciel/iterator_impl/advance.hpp>
#include <ciel/iterator_impl/distance.hpp>

NAMESPACE_CIEL_BEGIN

template<class ForwardIt, class T, class Compare>
[[nodiscard]] constexpr auto upper_bound(ForwardIt first, ForwardIt last, const T& value, Compare comp) -> ForwardIt {
    auto count = ciel::distance(first, last);
    while (count > 0) {
        ForwardIt it = first;
        auto step = count / 2;
        ciel::advance(it, step);
        if (!comp(value, *it)) {
            first = ++it;
            count -= step + 1;
        } else {
            count = step;
        }
    }
    return first;
}

template<class ForwardIt, class T>
[[nodiscard]] constexpr auto upper_bound(ForwardIt first, ForwardIt last, const T& value) -> ForwardIt {
    return ciel::upper_bound(first, last, value, less<>());
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_UPPER_BOUND_HPP_