#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_FILL_N_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_FILL_N_HPP_

#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

template<class OutputIt, class Size, class T>
constexpr auto fill_n(OutputIt first, Size count, const T& value) -> OutputIt {
    for (Size i = 0; i < count; ++i) {
        *first++ = value;
    }
    return first;
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_FILL_N_HPP_