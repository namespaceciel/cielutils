#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_POLYMORPHIC_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_POLYMORPHIC_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/integral_constant.hpp>

NAMESPACE_CIEL_BEGIN

namespace details {

// SFINAE: dynamic_cast rule: If expression is a pointer to a polymorphic type,
// and target-type is a pointer to void, the result is a pointer to the most
// derived object pointed or referenced by expression.
template<class T>
auto is_polymorphic_helper(decltype(dynamic_cast<const volatile void*>(static_cast<T*>(nullptr)))) -> true_type;

template<class T>
auto is_polymorphic_helper(...) -> false_type;

}   // namespace details

template<class T>
struct is_polymorphic : decltype(details::is_polymorphic_helper<T>(nullptr)) {};

template< class T >
inline constexpr bool is_polymorphic_v = is_polymorphic<T>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_POLYMORPHIC_HPP_