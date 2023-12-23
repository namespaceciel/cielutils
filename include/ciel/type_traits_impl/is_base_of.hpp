#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_BASE_OF_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_BASE_OF_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/is_class.hpp>

NAMESPACE_CIEL_BEGIN

namespace details {

template<class B>
auto is_convertible_to_base(const volatile B*) -> true_type;

template<class>
auto is_convertible_to_base(const volatile void*) -> false_type;

template<class Base, class Derived>
auto is_base_of_helper(int) -> decltype(is_convertible_to_base<Base>(static_cast<Derived*>(nullptr)));

// SFINAE: prevent the compile error when in private inheriting,
// derived pointer can't convert to the base pointer, dump the above specialization
template<class, class>
auto is_base_of_helper(...) -> true_type;

}   // namespace details

template<class Base, class Derived>
struct is_base_of : bool_constant<is_class_v<Base> && is_class_v<Derived> &&
        decltype(details::is_base_of_helper<Base, Derived>(0))::value> {};

template<class Base, class Derived>
inline constexpr bool is_base_of_v = is_base_of<Base, Derived>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_BASE_OF_HPP_