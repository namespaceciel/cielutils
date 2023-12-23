#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_LEXICOGRAPHICAL_COMPARE_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_LEXICOGRAPHICAL_COMPARE_HPP_

#include <ciel/config.hpp>
#include <ciel/functional_impl/less.hpp>

NAMESPACE_CIEL_BEGIN

template<class InputIt1, class InputIt2, class Compare>
[[nodiscard]] constexpr auto lexicographical_compare(InputIt1 first1, InputIt1 last1,
                                                     InputIt2 first2, InputIt2 last2, Compare comp) -> bool {
    for (; (first1 != last1) && (first2 != last2); ++first1, ++first2) {
        if (comp(*first1, *first2)) {
            return true;
        }
        if (comp(*first2, *first1)) {
            return false;
        }
    }
    return (first1 == last1) && (first2 != last2);
}

template<class InputIt1, class InputIt2>
[[nodiscard]] constexpr auto lexicographical_compare(InputIt1 first1, InputIt1 last1,
                                                     InputIt2 first2, InputIt2 last2) -> bool {
    return ciel::lexicographical_compare(first1, last1, first2, last2, less<>());
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_LEXICOGRAPHICAL_COMPARE_HPP_