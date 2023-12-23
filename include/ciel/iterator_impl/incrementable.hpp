#ifndef CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_INCREMENTABLE_HPP_
#define CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_INCREMENTABLE_HPP_

#include <ciel/concepts_impl/regular.hpp>
#include <ciel/config.hpp>
#include <ciel/iterator_impl/weakly_incrementable.hpp>

NAMESPACE_CIEL_BEGIN

template<class I>
concept incrementable = regular<I>
    && weakly_incrementable<I>
    && requires(I i) {
    { i++ } -> same_as<I>;
};

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_INCREMENTABLE_HPP_