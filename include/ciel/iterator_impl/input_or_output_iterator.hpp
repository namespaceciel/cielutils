#ifndef CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_INPUT_OR_OUTPUT_ITERATOR_HPP_
#define CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_INPUT_OR_OUTPUT_ITERATOR_HPP_

#include <ciel/config.hpp>
#include <ciel/iterator_impl/weakly_incrementable.hpp>

NAMESPACE_CIEL_BEGIN

template<class I>
concept input_or_output_iterator = requires(I i) {
    { *i } -> can_reference;
} && weakly_incrementable<I>;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_INPUT_OR_OUTPUT_ITERATOR_HPP_