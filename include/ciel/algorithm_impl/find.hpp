#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_FIND_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_FIND_HPP_

#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

template<class InputIt, class T>
[[nodiscard]] constexpr auto find(InputIt first, InputIt last, const T& value) -> InputIt {
    for (; first != last; ++first) {
        if (*first == value) {
            return first;
        }
    }
    return last;
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_FIND_HPP_