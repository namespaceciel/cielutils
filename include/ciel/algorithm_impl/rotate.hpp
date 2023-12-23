#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_ROTATE_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_ROTATE_HPP_

#include <ciel/algorithm_impl/iter_swap.hpp>
#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

template<class ForwardIt>
constexpr auto rotate(ForwardIt first, ForwardIt middle, ForwardIt last) -> ForwardIt {
    if (first == middle) {
        return last;
    }
    if (middle == last) {
        return first;
    }
    ForwardIt write = first;
    ForwardIt next_read = first;
    for (ForwardIt read = middle; read != last; ++write, ++read) {
        if (write == next_read) {
            next_read = read;
        }
        ciel::iter_swap(write, read);
    }
    ciel::rotate(write, next_read, last);
    return write;
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_ROTATE_HPP_