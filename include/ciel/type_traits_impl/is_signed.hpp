#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_SIGNED_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_SIGNED_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/is_arithmetic.hpp>

NAMESPACE_CIEL_BEGIN

namespace details {

template<class T, bool = is_arithmetic<T>::value>
struct is_signed_helper : bool_constant<T(-1) < T(0)> {};

template<class T>
struct is_signed_helper<T, false> : false_type {};

}   // namespace details

template<class T>
struct is_signed : details::is_signed_helper<T>::type {};

template<class T>
inline constexpr bool is_signed_v = is_signed<T>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_SIGNED_HPP_