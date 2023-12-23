#ifndef CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_WEAKLY_INCREMENTABLE_HPP_
#define CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_WEAKLY_INCREMENTABLE_HPP_

#include <ciel/concepts_impl/default_initializable.hpp>
#include <ciel/config.hpp>
#include <ciel/iterator_impl/iter_alias.hpp>

NAMESPACE_CIEL_BEGIN

template<class I>
concept weakly_incrementable = default_initializable<I>
    && movable<I>
    && requires(I i) {
        typename iter_difference_t<I>;
        requires signed_integral<iter_difference_t<I>>;
        { ++i } -> same_as<I&>;
        i++;
    };

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_WEAKLY_INCREMENTABLE_HPP_