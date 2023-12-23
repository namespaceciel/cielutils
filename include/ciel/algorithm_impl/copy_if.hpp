#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_COPY_IF_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_COPY_IF_HPP_

#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

// It's undefined behavior when two ranges overlap
template<class InputIt, class OutputIt, class UnaryPredicate>
constexpr auto copy_if(InputIt first, InputIt last, OutputIt d_first, UnaryPredicate pred) -> OutputIt {
    while (first != last) {
        if (pred(*first)) {
            *d_first++ = *first;
        }
        ++first;
    }
    return d_first;
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_COPY_IF_HPP_