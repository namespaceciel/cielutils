#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_COUNT_IF_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_COUNT_IF_HPP_

#include <ciel/config.hpp>
#include <ciel/iterator_impl/iterator_traits.hpp>

NAMESPACE_CIEL_BEGIN

template<class InputIt, class UnaryPredicate>
[[nodiscard]] constexpr auto count_if(InputIt first, InputIt last, UnaryPredicate p)
    -> typename iterator_traits<InputIt>::difference_type {
    typename iterator_traits<InputIt>::difference_type res = 0;
    for (; first != last; ++first) {
        if (p(*first)) {
            ++res;
        }
    }
    return res;
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_COUNT_IF_HPP_