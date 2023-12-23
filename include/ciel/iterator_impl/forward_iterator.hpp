#ifndef CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_FORWARD_ITERATOR_HPP_
#define CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_FORWARD_ITERATOR_HPP_

#include <ciel/config.hpp>
#include <ciel/iterator_impl/incrementable.hpp>
#include <ciel/iterator_impl/input_iterator.hpp>
#include <ciel/iterator_impl/iter_concept.hpp>
#include <ciel/iterator_impl/sentinel_for.hpp>

NAMESPACE_CIEL_BEGIN

template<class I>
concept forward_iterator = input_iterator<I>
    && derived_from<iter_concept<I>, forward_iterator_tag>
    && incrementable<I>
    && sentinel_for<I, I>;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_FORWARD_ITERATOR_HPP_