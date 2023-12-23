#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_COPY_N_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_COPY_N_HPP_

#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

template<class InputIt, class Size, class OutputIt>
constexpr auto copy_n(InputIt first, Size count, OutputIt result) -> OutputIt {
    if (count > 0) {
        *result++ = *first;
        for (Size i = 1; i < count; ++i) {
            *result++ = *++first;
        }
    }
    return result;
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_COPY_N_HPP_