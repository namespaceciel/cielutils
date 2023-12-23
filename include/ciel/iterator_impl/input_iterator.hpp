#ifndef CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_INPUT_ITERATOR_HPP_
#define CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_INPUT_ITERATOR_HPP_

#include <ciel/concepts_impl/derived_from.hpp>
#include <ciel/config.hpp>
#include <ciel/iterator_impl/indirectly_readable.hpp>
#include <ciel/iterator_impl/input_or_output_iterator.hpp>
#include <ciel/iterator_impl/iter_concept.hpp>

NAMESPACE_CIEL_BEGIN

template<class I>
concept input_iterator = input_or_output_iterator<I>
    && indirectly_readable<I>
    && requires { typename iter_concept<I>; }
    && derived_from<iter_concept<I>, input_iterator_tag>;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_INPUT_ITERATOR_HPP_