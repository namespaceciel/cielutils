#ifndef CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_MOVE_CONSTRUCTIBLE_HPP_
#define CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_MOVE_CONSTRUCTIBLE_HPP_

#include <ciel/concepts_impl/constructible_from.hpp>
#include <ciel/concepts_impl/convertible_to.hpp>
#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
concept move_constructible = constructible_from<T, T> && convertible_to<T, T>;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_MOVE_CONSTRUCTIBLE_HPP_