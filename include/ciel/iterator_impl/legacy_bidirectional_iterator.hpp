#ifndef CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_LEGACY_BIDIRECTIONAL_ITERATOR_HPP_
#define CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_LEGACY_BIDIRECTIONAL_ITERATOR_HPP_

#include <ciel/config.hpp>
#include <ciel/iterator_impl/legacy_forward_iterator.hpp>

NAMESPACE_CIEL_BEGIN

template<class I>
concept legacy_bidirectional_iterator = legacy_forward_iterator<I> && requires(I i) {
    { --i } -> same_as<I&>;
    { i-- } -> convertible_to<const I&>;
    { *i-- } -> same_as<iter_reference_t<I>>;
};

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_LEGACY_BIDIRECTIONAL_ITERATOR_HPP_