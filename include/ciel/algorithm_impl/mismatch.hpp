#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_MISMATCH_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_MISMATCH_HPP_

#include <ciel/config.hpp>
#include <ciel/functional_impl/equal_to.hpp>
#include <ciel/utility_impl/pair.hpp>

NAMESPACE_CIEL_BEGIN

template<class InputIt1, class InputIt2, class BinaryPredicate>
[[nodiscard]] constexpr auto mismatch(InputIt1 first1, InputIt1 last1, InputIt2 first2, BinaryPredicate p)
    -> pair<InputIt1,InputIt2> {
    while (first1 != last1 && p(*first1, *first2)) {
        ++first1;
        ++first2;
    }
    return {first1, first2};
}

template<class InputIt1, class InputIt2>
[[nodiscard]] constexpr auto mismatch(InputIt1 first1, InputIt1 last1, InputIt2 first2) -> pair<InputIt1,InputIt2> {
    return ciel::mismatch(first1, last1, first2, equal_to<>());
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_MISMATCH_HPP_