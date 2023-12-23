#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_COPY_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_COPY_HPP_

#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

// It's undefined behavior when d_first in range [first, last), use copy_backward instead
template<class InputIt, class OutputIt>
constexpr auto copy(InputIt first, InputIt last, OutputIt d_first) -> OutputIt {
    while (first != last) {
        *d_first++ = *first++;
    }
    return d_first;
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_COPY_HPP_