#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_ADD_POINTER_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_ADD_POINTER_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/remove_reference.hpp>
#include <ciel/type_traits_impl/type_identity.hpp>

NAMESPACE_CIEL_BEGIN

namespace details {

template<class T>
auto add_pointer_helper(int) -> type_identity<remove_reference_t<T>*>;

// SFINAE: call the following function when T is cv or ref-qualified function
// e.g.
// struct S {
//     add_pointer_t<void() const> f;
// };
template<class T>
auto add_pointer_helper(...) -> type_identity<T>;

}   // namespace details

template<class T>
struct add_pointer : decltype(details::add_pointer_helper<T>(0)) {};

template<class T>
using add_pointer_t = typename add_pointer<T>::type;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_ADD_POINTER_HPP_