#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_REMOVE_IF_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_REMOVE_IF_HPP_

#include <ciel/algorithm_impl/find_if.hpp>
#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

template<class ForwardIt, class UnaryPredicate>
auto remove_if(ForwardIt first, ForwardIt last, UnaryPredicate p) -> ForwardIt {
    first = ciel::find_if(first, last, p);
    if (first != last) {
        for (ForwardIt i = first; ++i != last;) {
            if (!p(*i)) {
                *first++ = std::move(*i);
            }
        }
    }
    return first;
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_REMOVE_IF_HPP_