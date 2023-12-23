#ifndef CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_PREV_HPP_
#define CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_PREV_HPP_

#include <ciel/config.hpp>
#include <ciel/iterator_impl/advance.hpp>

NAMESPACE_CIEL_BEGIN

template<class BidirIt>
constexpr auto prev(BidirIt it, typename iterator_traits<BidirIt>::difference_type n = 1) -> BidirIt {
    ciel::advance(it, -n);
    return it;
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_PREV_HPP_