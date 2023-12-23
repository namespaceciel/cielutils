#ifndef CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_CONTIGUOUS_ITERATOR_HPP_
#define CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_CONTIGUOUS_ITERATOR_HPP_

#include <ciel/config.hpp>
#include <ciel/iterator_impl/random_access_iterator.hpp>
#include <ciel/memory_impl/to_address.hpp>

NAMESPACE_CIEL_BEGIN

template<class I>
concept contiguous_iterator = random_access_iterator<I>
    && derived_from<iter_concept<I>, contiguous_iterator_tag>
    && is_lvalue_reference_v<iter_reference_t<I>>
    && same_as<iter_value_t<I>, remove_cvref_t<iter_reference_t<I>>>
    && requires(const I& i) {{ ciel::to_address(i) } -> same_as<add_pointer_t<iter_reference_t<I>>>; };

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_CONTIGUOUS_ITERATOR_HPP_