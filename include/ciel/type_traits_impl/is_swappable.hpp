#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_SWAPPABLE_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_SWAPPABLE_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/is_function.hpp>
#include <ciel/type_traits_impl/is_move_assignable.hpp>
#include <ciel/type_traits_impl/is_move_constructible.hpp>
#include <ciel/type_traits_impl/is_void.hpp>
#include <ciel/utility_impl/declval.hpp>

NAMESPACE_CIEL_BEGIN

namespace details {

template<class T, class U>
concept can_swap = requires {
    std::swap(declval<T&>(), declval<U&>());
    std::swap(declval<U&>(), declval<T&>());
};

template<class T, class U>
concept can_nothrow_swap = requires {
    { std::swap(declval<T&>(), declval<U&>()) } noexcept;
    { std::swap(declval<U&>(), declval<T&>()) } noexcept;
};

template<class, class>
struct is_swappable_with_helper : false_type {};

template<class T, class U>
    requires can_swap<T, U>
struct is_swappable_with_helper<T, U> : true_type {};

template<class, class>
struct is_nothrow_swappable_with_helper : false_type {};

template<class T, class U>
    requires can_nothrow_swap<T, U>
struct is_nothrow_swappable_with_helper<T, U> : true_type {};

}   // namespace details

template<class T, class U>
struct is_swappable_with : details::is_swappable_with_helper<T,U> {};

template<class T, class U>
inline constexpr bool is_swappable_with_v = is_swappable_with<T, U>::value;

template<class T>
struct is_swappable : bool_constant<is_swappable_with_v<T, T> && !is_void_v<T> && !is_function_v<T>> {};

template<class T>
inline constexpr bool is_swappable_v = is_swappable<T>::value;

template<class T, class U>
struct is_nothrow_swappable_with : details::is_nothrow_swappable_with_helper<T, U> {};

template<class T, class U>
inline constexpr bool is_nothrow_swappable_with_v = is_nothrow_swappable_with<T, U>::value;

template<class T>
struct is_nothrow_swappable : bool_constant<is_nothrow_swappable_with_v<T, T> && !is_void_v<T> && !is_function_v<T>> {};

template<class T>
inline constexpr bool is_nothrow_swappable_v = is_nothrow_swappable<T>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_SWAPPABLE_HPP_