#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_NEGATION_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_NEGATION_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/integral_constant.hpp>

NAMESPACE_CIEL_BEGIN

template<class B>
struct negation : bool_constant<!static_cast<bool>(B::value)> {};

template<class B>
inline constexpr bool negation_v = negation<B>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_NEGATION_HPP_