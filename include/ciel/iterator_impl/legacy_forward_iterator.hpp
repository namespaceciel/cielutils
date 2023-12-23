#ifndef CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_LEGACY_FORWARD_ITERATOR_HPP_
#define CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_LEGACY_FORWARD_ITERATOR_HPP_

#include <ciel/config.hpp>
#include <ciel/iterator_impl/legacy_input_iterator.hpp>

NAMESPACE_CIEL_BEGIN

template<class I>
concept legacy_forward_iterator = legacy_input_iterator<I> && constructible_from<I>
    && is_lvalue_reference_v<iter_reference_t<I>>
    && same_as<remove_cvref_t<iter_reference_t<I>>, typename indirectly_readable_traits<I>::value_type>
    && requires(I i) {
        { i++ } -> convertible_to<const I&>;
        { *i++ } -> same_as<iter_reference_t<I>>;
    };

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_LEGACY_FORWARD_ITERATOR_HPP_