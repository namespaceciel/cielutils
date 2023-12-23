#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_FIND_IF_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_FIND_IF_HPP_

#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

template<class InputIt, class UnaryPredicate>
[[nodiscard]] constexpr auto find_if(InputIt first, InputIt last, UnaryPredicate p) -> InputIt {
    for (; first != last; ++first) {
        if (p(*first)) {
            return first;
        }
    }
    return last;
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_FIND_IF_HPP_