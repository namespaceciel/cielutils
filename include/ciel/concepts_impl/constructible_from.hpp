#ifndef CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_CONSTRUCTIBLE_FROM_HPP_
#define CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_CONSTRUCTIBLE_FROM_HPP_

#include <ciel/concepts_impl/destructible.hpp>
#include <ciel/config.hpp>
#include <ciel/type_traits_impl/is_constructible.hpp>

NAMESPACE_CIEL_BEGIN

template<class T, class... Args>
concept constructible_from = destructible<T> && is_constructible_v<T, Args...>;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_CONSTRUCTIBLE_FROM_HPP_