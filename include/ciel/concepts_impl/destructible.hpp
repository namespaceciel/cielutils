#ifndef CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_DESTRUCTIBLE_HPP_
#define CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_DESTRUCTIBLE_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/is_destructible.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
concept destructible = is_nothrow_destructible_v<T>;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_DESTRUCTIBLE_HPP_