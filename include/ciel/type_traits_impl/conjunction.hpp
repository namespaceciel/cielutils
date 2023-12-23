#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_CONJUNCTION_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_CONJUNCTION_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/conditional.hpp>
#include <ciel/type_traits_impl/integral_constant.hpp>

NAMESPACE_CIEL_BEGIN

// When the template pack is empty, derive from true_type
template<class...>
struct conjunction : true_type {};

// Otherwise, derive from the first false template member (if all true, choose the last one)
template<class B1>
struct conjunction<B1> : B1 {};

template<class B1, class... Bn>
struct conjunction<B1, Bn...> : conditional_t<static_cast<bool>(B1::value), conjunction<Bn...>, B1> {};

template<class... B>
inline constexpr bool conjunction_v = conjunction<B...>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_CONJUNCTION_HPP_