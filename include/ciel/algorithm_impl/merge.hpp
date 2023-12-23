#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_MERGE_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_MERGE_HPP_

#include <ciel/algorithm_impl/copy.hpp>
#include <ciel/config.hpp>
#include <ciel/functional_impl/less.hpp>

NAMESPACE_CIEL_BEGIN

template<class InputIt1, class InputIt2, class OutputIt, class Compare>
[[nodiscard]] constexpr auto merge(InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                   InputIt2 last2, OutputIt d_first, Compare comp ) -> OutputIt {
    for (; first1 != last1; ++d_first) {
        if (first2 == last2) {
            return ciel::copy(first1, last1, d_first);
        }
        if (comp(*first2, *first1)) {
            *d_first = *first2;
            ++first2;
        } else {
            *d_first = *first1;
            ++first1;
        }
    }
    return ciel::copy(first2, last2, d_first);
}

template<class InputIt1, class InputIt2, class OutputIt>
[[nodiscard]] constexpr auto merge(InputIt1 first1, InputIt1 last1,
                                   InputIt2 first2, InputIt2 last2, OutputIt d_first) -> OutputIt {
    return ciel::merge(first1, last1, first2, last2, d_first, less<>());
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_MERGE_HPP_