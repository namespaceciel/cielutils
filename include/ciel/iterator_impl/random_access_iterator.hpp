#ifndef CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_RANDOM_ACCESS_ITERATOR_HPP_
#define CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_RANDOM_ACCESS_ITERATOR_HPP_

#include <ciel/config.hpp>
#include <ciel/iterator_impl/bidirectional_iterator.hpp>
#include <ciel/iterator_impl/sized_sentinel_for.hpp>

NAMESPACE_CIEL_BEGIN

template<class I>
concept random_access_iterator = bidirectional_iterator<I>
    && derived_from<iter_concept<I>, random_access_iterator_tag>
    && totally_ordered<I>
    && sized_sentinel_for<I, I>
    && requires(I i, const I j, const iter_difference_t<I> n) {
        { i += n } -> same_as<I&>;
        { j + n } -> same_as<I>;
        { n + j } -> same_as<I>;
        { i -= n } -> same_as<I&>;
        { j - n } -> same_as<I>;
        { j[n] } -> same_as<iter_reference_t<I>>;
    };

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_RANDOM_ACCESS_ITERATOR_HPP_