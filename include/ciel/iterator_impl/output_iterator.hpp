#ifndef CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_OUTPUT_ITERATOR_HPP_
#define CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_OUTPUT_ITERATOR_HPP_

#include <ciel/config.hpp>
#include <ciel/iterator_impl/indirectly_writable.hpp>
#include <ciel/iterator_impl/input_or_output_iterator.hpp>

NAMESPACE_CIEL_BEGIN

template<class I, class T>
concept output_iterator = input_or_output_iterator<I>
    && indirectly_writable<I, T>
    && requires(I i, T&& t) { *i++ = std::forward<T>(t); };

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_OUTPUT_ITERATOR_HPP_