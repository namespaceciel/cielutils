#ifndef CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_SENTINEL_FOR_HPP_
#define CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_SENTINEL_FOR_HPP_

#include <ciel/concepts_impl/semiregular.hpp>
#include <ciel/config.hpp>
#include <ciel/iterator_impl/input_or_output_iterator.hpp>

NAMESPACE_CIEL_BEGIN

template<class S, class I>
concept sentinel_for = semiregular<S>
    && input_or_output_iterator<I>
    && details::weakly_equality_comparable_with<S, I>;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_SENTINEL_FOR_HPP_