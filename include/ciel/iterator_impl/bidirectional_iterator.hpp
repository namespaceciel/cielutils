#ifndef CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_BIDIRECTIONAL_ITERATOR_HPP_
#define CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_BIDIRECTIONAL_ITERATOR_HPP_

#include <ciel/config.hpp>
#include <ciel/iterator_impl/forward_iterator.hpp>

NAMESPACE_CIEL_BEGIN

template<class I>
concept bidirectional_iterator = forward_iterator<I>
    && derived_from<iter_concept<I>, bidirectional_iterator_tag>
    && requires(I i) {
        { --i } -> same_as<I&>;
        { i-- } -> same_as<I>;
    };

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_BIDIRECTIONAL_ITERATOR_HPP_