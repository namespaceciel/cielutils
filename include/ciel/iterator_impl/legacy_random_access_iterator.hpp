#ifndef CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_LEGACY_RANDOM_ACCESS_ITERATOR_HPP_
#define CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_LEGACY_RANDOM_ACCESS_ITERATOR_HPP_

#include <ciel/concepts_impl/totally_ordered.hpp>
#include <ciel/config.hpp>
#include <ciel/iterator_impl/legacy_bidirectional_iterator.hpp>

NAMESPACE_CIEL_BEGIN

template<class I>
concept legacy_random_access_iterator = legacy_bidirectional_iterator<I>
    && totally_ordered<I>
    && requires(I i, typename incrementable_traits<I>::difference_type n) {
        { i += n } -> same_as<I&>;
        { i -= n } -> same_as<I&>;
        { i + n } -> same_as<I>;
        { n + i } -> same_as<I>;
        { i - n } -> same_as<I>;
        { i - i } -> same_as<decltype(n)>;
        { i[n] } -> convertible_to<iter_reference_t<I>>;
    };

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_LEGACY_RANDOM_ACCESS_ITERATOR_HPP_