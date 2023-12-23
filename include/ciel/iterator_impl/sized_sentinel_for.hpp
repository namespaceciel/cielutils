#ifndef CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_SIZED_SENTINEL_FOR_HPP_
#define CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_SIZED_SENTINEL_FOR_HPP_

#include <ciel/config.hpp>
#include <ciel/iterator_impl/sentinel_for.hpp>

NAMESPACE_CIEL_BEGIN

template<class, class>
inline constexpr bool disable_sized_sentinel_for = false;

template<class S, class I>
concept sized_sentinel_for = sentinel_for<S, I>
    && !disable_sized_sentinel_for<remove_cv_t<S>, remove_cv_t<I>>
    && requires(const I& i, const S& s) {
        { s - i } -> same_as<iter_difference_t<I>>;
        { i - s } -> same_as<iter_difference_t<I>>;
    };

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_SIZED_SENTINEL_FOR_HPP_