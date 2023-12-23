#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_POINTER_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_POINTER_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/integral_constant.hpp>
#include <ciel/type_traits_impl/remove_cv.hpp>

NAMESPACE_CIEL_BEGIN

// std::nullptr_t is not pointer type, but can convert to any pointer type

namespace details {

template<class T>
struct is_pointer_helper : false_type {};

template<class T>
struct is_pointer_helper<T*> : true_type {};

}   // namespace details

template<class T>
struct is_pointer : details::is_pointer_helper<remove_cv_t<T>> {};

template<class T>
inline constexpr bool is_pointer_v = is_pointer<T>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_POINTER_HPP_