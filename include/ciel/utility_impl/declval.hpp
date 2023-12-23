#ifndef CIELUTILS_INCLUDE_CIEL_UTILITY_IMPL_DECLVAL_HPP_
#define CIELUTILS_INCLUDE_CIEL_UTILITY_IMPL_DECLVAL_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/add_rvalue_reference.hpp>

NAMESPACE_CIEL_BEGIN

// convert any type T to reference, so that you can call member function without constructing it,
// only can be used in unevaluated contexts
template<class T>
auto declval() noexcept -> add_rvalue_reference_t<T>;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_UTILITY_IMPL_DECLVAL_HPP_