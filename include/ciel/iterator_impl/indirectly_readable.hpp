#ifndef CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_INDIRECTLY_READABLE_HPP_
#define CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_INDIRECTLY_READABLE_HPP_

#include <ciel/config.hpp>
#include <ciel/iterator_impl/iter_alias.hpp>

NAMESPACE_CIEL_BEGIN

template<class In>
concept indirectly_readable_helper = requires(const In in) {
    typename iter_value_t<In>;
    typename iter_reference_t<In>;
    typename iter_rvalue_reference_t<In>;
    { *in } -> same_as<iter_reference_t<In>>;
    { ranges::iter_move(in) } -> same_as<iter_rvalue_reference_t<In>>;
} && common_reference_with<iter_reference_t<In>&&, iter_value_t<In>&>
  && common_reference_with<iter_reference_t<In>&&, iter_rvalue_reference_t<In>&&>
  && common_reference_with<iter_rvalue_reference_t<In>&&, const iter_value_t<In>&>;

template<class In>
concept indirectly_readable = indirectly_readable_helper<remove_cvref_t<In>>;

template<indirectly_readable T>
using iter_common_reference_t = common_reference_t<iter_reference_t<T>, iter_value_t<T>&>;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_INDIRECTLY_READABLE_HPP_