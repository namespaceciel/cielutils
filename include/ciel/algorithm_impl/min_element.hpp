#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_MIN_ELEMENT_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_MIN_ELEMENT_HPP_

#include <ciel/config.hpp>
#include <ciel/functional_impl/less.hpp>

NAMESPACE_CIEL_BEGIN

template<class ForwardIt, class Compare>
[[nodiscard]] constexpr auto min_element(ForwardIt first, ForwardIt last, Compare comp) -> ForwardIt {
    if (first == last) {
        return last;
    }
    ForwardIt smallest = first++;
    for (; first != last; ++first) {
        if (comp(*first, *smallest)) {
            smallest = first;
        }
    }
    return smallest;
}

template<class ForwardIt>
[[nodiscard]] constexpr auto min_element(ForwardIt first, ForwardIt last) -> ForwardIt {
    return ciel::min_element(first, last, less<>());
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_MIN_ELEMENT_HPP_