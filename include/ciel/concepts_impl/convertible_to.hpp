#ifndef CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_CONVERTIBLE_TO_HPP_
#define CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_CONVERTIBLE_TO_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/is_convertible.hpp>
#include <ciel/utility_impl/declval.hpp>

NAMESPACE_CIEL_BEGIN

template<class From, class To>
concept convertible_to = is_convertible_v<From, To> && requires { static_cast<To>(declval<From>()); };

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_CONVERTIBLE_TO_HPP_