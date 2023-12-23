#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_GENERATE_N_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_GENERATE_N_HPP_

#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

template<class OutputIt, class Size, class Generator>
constexpr auto generate_n(OutputIt first, Size count, Generator g) -> OutputIt {
    for (Size i = 0; i < count; ++i) {
        *first++ = g();
    }
    return first;
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_GENERATE_N_HPP_