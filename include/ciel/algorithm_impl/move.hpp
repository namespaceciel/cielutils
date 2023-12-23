#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_MOVE_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_MOVE_HPP_

#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

// It's undefined behavior when d_first in range [first, last), use move_backward instead
template<class InputIt, class OutputIt>
constexpr auto move(InputIt first, InputIt last, OutputIt d_first) -> OutputIt {
    while (first != last) {
        *d_first++ = std::move(*first++);
    }
    return d_first;
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_MOVE_HPP_