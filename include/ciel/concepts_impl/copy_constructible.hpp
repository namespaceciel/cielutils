#ifndef CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_COPY_CONSTRUCTIBLE_HPP_
#define CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_COPY_CONSTRUCTIBLE_HPP_

#include <ciel/concepts_impl/move_constructible.hpp>
#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
concept copy_constructible = move_constructible<T>
    && constructible_from<T, T&>
    && convertible_to<T&, T>
    && constructible_from<T, const T&>
    && convertible_to<const T&, T>
    && constructible_from<T, const T>
    && convertible_to<const T, T>;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_COPY_CONSTRUCTIBLE_HPP_