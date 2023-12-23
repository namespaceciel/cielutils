#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_COMPOUND_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_COMPOUND_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/is_fundamental.hpp>

NAMESPACE_CIEL_BEGIN

// Every C++ type is neither fundamental nor compound
template<class T>
struct is_compound : bool_constant<!is_fundamental_v<T>> {};

template<class T>
inline constexpr bool is_compound_v = is_compound<T>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_COMPOUND_HPP_