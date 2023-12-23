#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_TRANSFORM_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_TRANSFORM_HPP_

#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

template<class InputIt, class OutputIt, class UnaryOperation>
constexpr auto transform(InputIt first1, InputIt last1, OutputIt d_first, UnaryOperation unary_op) -> OutputIt {
    while (first1 != last1) {
        *d_first++ = unary_op(*first1++);
    }
    return d_first;
}

template<class InputIt1, class InputIt2, class OutputIt, class BinaryOperation>
constexpr auto transform(InputIt1 first1, InputIt1 last1, InputIt2 first2, OutputIt d_first, BinaryOperation binary_op)
    -> OutputIt {
    while (first1 != last1) {
        *d_first++ = binary_op(*first1++, *first2++);
    }
    return d_first;
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_TRANSFORM_HPP_