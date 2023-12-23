#ifndef CIELUTILS_INCLUDE_CIEL_NUMERIC_IMPL_REDUCE_HPP_
#define CIELUTILS_INCLUDE_CIEL_NUMERIC_IMPL_REDUCE_HPP_

#include <ciel/config.hpp>
#include <ciel/functional_impl/plus.hpp>
#include <ciel/iterator_impl/iterator_traits.hpp>

NAMESPACE_CIEL_BEGIN

template<class InputIt, class T, class BinaryOp>
constexpr auto reduce(InputIt first, InputIt last, T init, BinaryOp binary_op) -> T {
    for (; first != last; ++first) {
        init = binary_op(init, *first);
    }
    return init;
}

template<class InputIt, class T>
constexpr auto reduce(InputIt first, InputIt last, T init) -> T {
    return ciel::reduce(first, last, init, ciel::plus<>());
}

template<class InputIt>
constexpr auto reduce(InputIt first, InputIt last) -> typename iterator_traits<InputIt>::value_type {
    return ciel::reduce(first, last, typename iterator_traits<InputIt>::value_type{});
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_NUMERIC_IMPL_REDUCE_HPP_