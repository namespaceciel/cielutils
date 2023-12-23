#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_EQUAL_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_EQUAL_HPP_

#include <ciel/config.hpp>
#include <ciel/functional_impl/equal_to.hpp>

NAMESPACE_CIEL_BEGIN

template<class InputIt1, class InputIt2, class BinaryPredicate>
[[nodiscard]] constexpr auto equal(InputIt1 first1, InputIt1 last1, InputIt2 first2, BinaryPredicate p) -> bool {
    for (; first1 != last1; ++first1, ++first2) {
        if (!p(*first1, *first2)) {
            return false;
        }
    }
    return true;
}

template<class InputIt1, class InputIt2>
[[nodiscard]] constexpr auto equal(InputIt1 first1, InputIt1 last1, InputIt2 first2) -> bool {
    return ciel::equal(first1, last1, first2, equal_to<>());
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_EQUAL_HPP_