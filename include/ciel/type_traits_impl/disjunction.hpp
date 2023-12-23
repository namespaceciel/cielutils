#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_DISJUNCTION_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_DISJUNCTION_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/conditional.hpp>
#include <ciel/type_traits_impl/integral_constant.hpp>

NAMESPACE_CIEL_BEGIN

template<class...>
struct disjunction : false_type {};

template<class B1>
struct disjunction<B1> : B1 {};

template<class B1, class... Bn>
struct disjunction<B1, Bn...> : conditional_t<static_cast<bool>(B1::value), B1, disjunction<Bn...>> {};

template<class... B>
inline constexpr bool disjunction_v = disjunction<B...>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_DISJUNCTION_HPP_