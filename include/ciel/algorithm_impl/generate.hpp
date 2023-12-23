#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_GENERATE_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_GENERATE_HPP_

#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

template<class ForwardIt, class Generator>
constexpr auto generate(ForwardIt first, ForwardIt last, Generator g) -> void {
    while (first != last) {
        *first++ = g();
    }
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_GENERATE_HPP_