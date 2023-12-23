#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_REMOVE_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_REMOVE_HPP_

#include <ciel/algorithm_impl/find.hpp>
#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

template<class ForwardIt, class T>
constexpr auto remove(ForwardIt first, ForwardIt last, const T& value) -> ForwardIt {
    first = ciel::find(first, last, value);
    if (first != last) {
        for (ForwardIt i = first; ++i != last;) {
            if (!(*i == value)) {
                *first++ = std::move(*i);
            }
        }
    }
    return first;
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_REMOVE_HPP_