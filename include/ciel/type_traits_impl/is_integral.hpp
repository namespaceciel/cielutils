#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_INTEGRAL_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_INTEGRAL_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/integral_constant.hpp>
#include <ciel/type_traits_impl/remove_cv.hpp>

NAMESPACE_CIEL_BEGIN

namespace details {

template<class T>
struct is_integral_helper : false_type {};

template<>
struct is_integral_helper<bool> : true_type {};

template<>
struct is_integral_helper<char> : true_type {};

template<>
struct is_integral_helper<signed char> : true_type {};

template<>
struct is_integral_helper<unsigned char> : true_type {};

template<>
struct is_integral_helper<wchar_t> : true_type {};

template<>
struct is_integral_helper<char8_t> : true_type {};

template<>
struct is_integral_helper<char16_t> : true_type {};

template<>
struct is_integral_helper<char32_t> : true_type {};

template<>
struct is_integral_helper<short> : true_type {};

template<>
struct is_integral_helper<unsigned short> : true_type {};

template<>
struct is_integral_helper<int> : true_type {};

template<>
struct is_integral_helper<unsigned int> : true_type {};

template<>
struct is_integral_helper<long> : true_type {};

template<>
struct is_integral_helper<unsigned long> : true_type {};

template<>
struct is_integral_helper<long long> : true_type {};

template<>
struct is_integral_helper<unsigned long long> : true_type {};

}   // namespace details

template<class T>
struct is_integral : details::is_integral_helper<remove_cv_t<T>> {};

template<class T>
inline constexpr bool is_integral_v = is_integral<T>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_INTEGRAL_HPP_