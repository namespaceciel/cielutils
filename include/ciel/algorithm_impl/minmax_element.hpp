#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_MINMAX_ELEMENT_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_MINMAX_ELEMENT_HPP_

#include <ciel/config.hpp>
#include <ciel/functional_impl/less.hpp>
#include <ciel/utility_impl/pair.hpp>

NAMESPACE_CIEL_BEGIN

// This algorithm finds the last biggest element of the range, whereas max_element finds the first
template<class ForwardIt, class Compare>
[[nodiscard]] constexpr auto minmax_element(ForwardIt first, ForwardIt last, Compare comp)
    -> pair<ForwardIt,ForwardIt> {
    ForwardIt min = first;
    ForwardIt max = first;
    if (first == last || ++first == last) {
        return {min, max};
    }
    if (comp(*first, *min)) {
        min = first;
    } else {
        max = first;
    }

    while (++first != last) {
        ForwardIt it = first;
        if (++first == last) {
            if (comp(*it, *min)) {
                min = it;
            } else if (!(comp(*it, *max))) {
                max = it;
            }
            break;
        } else {
            if (comp(*first, *it)) {
                if (comp(*first, *min)) {
                    min = first;
                }
                if (!(comp(*it, *max))) {
                    max = it;
                }
            } else {
                if (comp(*it, *min)) {
                    min = it;
                }
                if (!(comp(*first, *max))) {
                    max = first;
                }
            }
        }
    }
    return {min, max};
}

template<class ForwardIt>
[[nodiscard]] constexpr auto minmax_element(ForwardIt first, ForwardIt last) -> pair<ForwardIt,ForwardIt> {
    return ciel::minmax_element(first, last, less<>());
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_MINMAX_ELEMENT_HPP_