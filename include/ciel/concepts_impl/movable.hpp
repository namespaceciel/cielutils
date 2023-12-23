#ifndef CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_MOVABLE_HPP_
#define CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_MOVABLE_HPP_

#include <ciel/concepts_impl/assignable_from.hpp>
#include <ciel/concepts_impl/move_constructible.hpp>
#include <ciel/concepts_impl/swappable.hpp>
#include <ciel/config.hpp>
#include <ciel/type_traits_impl/is_object.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
concept movable = is_object_v<T> && move_constructible<T> && assignable_from<T&, T> && swappable<T>;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_MOVABLE_HPP_