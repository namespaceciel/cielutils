#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_IS_SORTED_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_IS_SORTED_HPP_

#include <ciel/algorithm_impl/is_sorted_until.hpp>
#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

template<class ForwardIt, class Compare>
[[nodiscard]] auto is_sorted(ForwardIt first, ForwardIt last, Compare comp) -> bool {
    return ciel::is_sorted_until(first, last, comp) == last;
}

template<class ForwardIt>
[[nodiscard]] auto is_sorted(ForwardIt first, ForwardIt last) -> bool {
    return ciel::is_sorted_until(first, last) == last;
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_IS_SORTED_HPP_