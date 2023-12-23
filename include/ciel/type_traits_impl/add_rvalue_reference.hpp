#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_ADD_RVALUE_REFERENCE_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_ADD_RVALUE_REFERENCE_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/type_identity.hpp>

NAMESPACE_CIEL_BEGIN

namespace details {

template<class T>
auto add_rvalue_reference_helper(int) -> type_identity<T&&>;

// SFINAE: prevent the compile error from void to void&&, dump the above specialization
template<class T>
auto add_rvalue_reference_helper(...) -> type_identity<T>;

}   // namespace details

template<class T>
struct add_rvalue_reference : decltype(details::add_rvalue_reference_helper<T>(0)) {};

template<class T>
using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_ADD_RVALUE_REFERENCE_HPP_