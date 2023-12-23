#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_MAX_ELEMENT_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_MAX_ELEMENT_HPP_

#include <ciel/config.hpp>
#include <ciel/functional_impl/less.hpp>

NAMESPACE_CIEL_BEGIN

template<class ForwardIt, class Compare>
[[nodiscard]] constexpr auto max_element(ForwardIt first, ForwardIt last, Compare comp) -> ForwardIt {
    if (first == last) {
        return last;
    }
    ForwardIt largest = first++;
    for (; first != last; ++first) {
        if (comp(*largest, *first)) {
            largest = first;
        }
    }
    return largest;
}

template<class ForwardIt>
[[nodiscard]] constexpr auto max_element(ForwardIt first, ForwardIt last) -> ForwardIt {
    return ciel::max_element(first, last, less<>());
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_MAX_ELEMENT_HPP_