#ifndef CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_LEGACY_INPUT_ITERATOR_HPP_
#define CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_LEGACY_INPUT_ITERATOR_HPP_

#include <ciel/concepts_impl/equality_comparable.hpp>
#include <ciel/concepts_impl/signed_integral.hpp>
#include <ciel/config.hpp>
#include <ciel/iterator_impl/incrementable_traits.hpp>
#include <ciel/iterator_impl/indirectly_readable_traits.hpp>
#include <ciel/iterator_impl/legacy_iterator.hpp>

NAMESPACE_CIEL_BEGIN

template<class I>
concept legacy_input_iterator = legacy_iterator<I> && equality_comparable<I> && requires(I i) {
    typename incrementable_traits<I>::difference_type;
    typename indirectly_readable_traits<I>::value_type;
    typename common_reference_t<iter_reference_t<I>&&, typename indirectly_readable_traits<I>::value_type&>;
    *i++;
    typename common_reference_t<decltype(*i++)&&, typename indirectly_readable_traits<I>::value_type&>;
    requires signed_integral<typename incrementable_traits<I>::difference_type>;
};

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_LEGACY_INPUT_ITERATOR_HPP_