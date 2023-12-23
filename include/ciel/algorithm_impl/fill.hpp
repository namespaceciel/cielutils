#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_FILL_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_FILL_HPP_

#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

template<class ForwardIt, class T>
constexpr auto fill(ForwardIt first, ForwardIt last, const T& value) -> void {
    for (; first != last; ++first) {
        *first = value;
    }
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_FILL_HPP_