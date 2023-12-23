#ifndef CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_LEGACY_ITERATOR_HPP_
#define CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_LEGACY_ITERATOR_HPP_

#include <ciel/concepts_impl/copyable.hpp>
#include <ciel/config.hpp>
#include <ciel/iterator_impl/referenceable.hpp>

NAMESPACE_CIEL_BEGIN

template<class I>
concept legacy_iterator = requires(I i) {
    { *i } -> can_reference;
    { ++i } -> same_as<I&>;
    { *i++ } -> can_reference;
} && copyable<I>;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_LEGACY_ITERATOR_HPP_