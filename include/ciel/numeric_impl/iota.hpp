#ifndef CIELUTILS_INCLUDE_CIEL_NUMERIC_IMPL_IOTA_HPP_
#define CIELUTILS_INCLUDE_CIEL_NUMERIC_IMPL_IOTA_HPP_

#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

template<class ForwardIt, class T>
constexpr auto iota(ForwardIt first, ForwardIt last, T value) -> void {
    while (first != last) {
        *first++ = value;
        ++value;
    }
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_NUMERIC_IMPL_IOTA_HPP_