#ifndef CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_COPYABLE_HPP_
#define CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_COPYABLE_HPP_

#include <ciel/concepts_impl/copy_constructible.hpp>
#include <ciel/concepts_impl/movable.hpp>
#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
concept copyable = copy_constructible<T>
    && movable<T>
    && assignable_from<T&, T&>
    && assignable_from<T&, const T&>
    && assignable_from<T&, const T>;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_COPYABLE_HPP_