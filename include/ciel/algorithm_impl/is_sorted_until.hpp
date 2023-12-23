#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_IS_SORTED_UNTIL_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_IS_SORTED_UNTIL_HPP_

#include <ciel/config.hpp>
#include <ciel/functional_impl/less.hpp>

NAMESPACE_CIEL_BEGIN

template<class ForwardIt, class Compare>
[[nodiscard]] auto is_sorted_until(ForwardIt first, ForwardIt last, Compare comp) -> ForwardIt {
    if (first != last) {
        ForwardIt next = first;
        while (++next != last) {
            if (comp(*next, *first)) {
                return next;
            }
            first = next;
        }
    }
    return last;
}

template<class ForwardIt>
[[nodiscard]] auto is_sorted_until(ForwardIt first, ForwardIt last) -> ForwardIt {
    return ciel::is_sorted_until(first, last, less<>());
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_IS_SORTED_UNTIL_HPP_