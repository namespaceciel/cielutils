#ifndef CIELUTILS_INCLUDE_CIEL_NUMERIC_IMPL_ACCUMULATE_HPP_
#define CIELUTILS_INCLUDE_CIEL_NUMERIC_IMPL_ACCUMULATE_HPP_

#include <ciel/config.hpp>
#include <utility>

NAMESPACE_CIEL_BEGIN

template<class InputIt, class T, class BinaryOperation>
constexpr auto accumulate(InputIt first, InputIt last, T init, BinaryOperation op) -> T {
    for (; first != last; ++first) {
        init = op(std::move(init), *first);
    }
    return init;
}

template<class InputIt, class T>
constexpr auto accumulate(InputIt first, InputIt last, T init) -> T {
    for (; first != last; ++first) {
        init = std::move(init) + *first;
    }
    return init;
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_NUMERIC_IMPL_ACCUMULATE_HPP_