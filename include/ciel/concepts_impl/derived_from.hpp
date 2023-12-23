#ifndef CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_DERIVED_FROM_HPP_
#define CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_DERIVED_FROM_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/is_base_of.hpp>
#include <ciel/type_traits_impl/is_convertible.hpp>

NAMESPACE_CIEL_BEGIN

// Note that when Base is the private or protected base of Derived, it's different from is_base_of
template<class Derived, class Base>
concept derived_from = is_base_of_v<Base, Derived> && is_convertible_v<const volatile Derived*, const volatile Base*>;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_DERIVED_FROM_HPP_