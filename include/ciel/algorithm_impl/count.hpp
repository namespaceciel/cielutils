#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_COUNT_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_COUNT_HPP_

#include <ciel/config.hpp>
#include <ciel/iterator_impl/iterator_traits.hpp>

NAMESPACE_CIEL_BEGIN

template<class InputIt, class T>
[[nodiscard]] constexpr auto count(InputIt first, InputIt last, const T &value)
    -> typename iterator_traits<InputIt>::difference_type {
    typename iterator_traits<InputIt>::difference_type res = 0;
    for (; first != last; ++first) {
        if (*first == value) {
            ++res;
        }
    }
    return res;
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_COUNT_HPP_