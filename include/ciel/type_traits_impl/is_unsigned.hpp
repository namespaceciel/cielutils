#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_UNSIGNED_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_UNSIGNED_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/is_arithmetic.hpp>

NAMESPACE_CIEL_BEGIN

namespace details {

template<class T, bool = is_arithmetic<T>::value>
struct is_unsigned_helper : bool_constant<T(0) < T(-1)> {};	// T(-1) > T(0) will make pair with the previous <

template<class T>
struct is_unsigned_helper<T, false> : false_type {};

}   // namespace details

template<class T>
struct is_unsigned : details::is_unsigned_helper<T>::type {};

template<class T>
inline constexpr bool is_unsigned_v = is_unsigned<T>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_UNSIGNED_HPP_