#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_CONVERTIBLE_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_CONVERTIBLE_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/is_void.hpp>
#include <ciel/utility_impl/declval.hpp>

NAMESPACE_CIEL_BEGIN

namespace details {

template<class To, class = decltype(static_cast<To(*)()>(nullptr))>
auto test_returnable(int) -> true_type;

template<class>
auto test_returnable(...) -> false_type;

template<class From, class To, class = decltype(declval<void(&)(To)>()(declval<From>()))>
auto test_implicitly_convertible(int) -> true_type;

template<class, class>
auto test_implicitly_convertible(...) -> false_type;

}   // namespace details

template<class From, class To>
struct is_convertible : bool_constant<
    ((decltype(details::test_returnable<To>(0))::value &&
        decltype(details::test_implicitly_convertible<From, To>(0))::value))
    || (is_void_v<From> && is_void_v<To>)> {};

template<class From, class To>
inline constexpr bool is_convertible_v = is_convertible<From, To>::value;

template<class From, class To>
struct is_nothrow_convertible : bool_constant<is_void_v<From> && is_void_v<To>> {};

namespace details {

template<class From, class To>
concept test_no_throw_convertible = requires {
    static_cast<To(*)()>(nullptr);
    { declval<void (&)(To) noexcept>()(declval<From>()) } noexcept;
};

}   // namespace details

template<class From, class To>
    requires details::test_no_throw_convertible<From, To>
struct is_nothrow_convertible<From, To> : true_type {};

template<class From, class To>
inline constexpr bool is_nothrow_convertible_v = is_nothrow_convertible<From, To>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_CONVERTIBLE_HPP_