#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_MEMBER_FUNCTION_POINTER_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_MEMBER_FUNCTION_POINTER_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/is_function.hpp>
#include <ciel/type_traits_impl/remove_cv.hpp>

NAMESPACE_CIEL_BEGIN

namespace details {

template<class T>
struct is_member_function_pointer_helper : false_type {};

template<class T, class U>
struct is_member_function_pointer_helper<T U::*> : is_function<T> {};

}   // namespace details

template<class T>
struct is_member_function_pointer : details::is_member_function_pointer_helper<remove_cv_t<T> > {};

template<class T>
inline constexpr bool is_member_function_pointer_v = is_member_function_pointer<T>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_MEMBER_FUNCTION_POINTER_HPP_