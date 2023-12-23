#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_IMPLICITLY_DEFAULT_CONSTRUCTIBLE_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_IMPLICITLY_DEFAULT_CONSTRUCTIBLE_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/is_default_constructible.hpp>

NAMESPACE_CIEL_BEGIN

namespace details {

template<class T>
auto is_implicitly_default_constructible_helper(T) -> void;

}   // namespace details

template<class T, class = void, class = typename is_default_constructible<T>::type>
struct is_implicitly_default_constructible : false_type {};

template<class T>
struct is_implicitly_default_constructible<T,
    decltype(details::is_implicitly_default_constructible_helper<const T&>({})), true_type> : true_type {};

template<class T>
struct is_implicitly_default_constructible<T,
    decltype(details::is_implicitly_default_constructible_helper<const T&>({})), false_type> : false_type {};

template<class T>
inline constexpr bool is_implicitly_default_constructible_v = is_implicitly_default_constructible<T>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_IMPLICITLY_DEFAULT_CONSTRUCTIBLE_HPP_