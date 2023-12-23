#ifndef CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_ASSIGNABLE_FROM_HPP_
#define CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_ASSIGNABLE_FROM_HPP_

#include <ciel/concepts_impl/common_reference_with.hpp>
#include <ciel/config.hpp>
#include <ciel/type_traits_impl/is_lvalue_reference.hpp>
#include <ciel/type_traits_impl/remove_reference.hpp>

NAMESPACE_CIEL_BEGIN

template<class LHS, class RHS>
concept assignable_from = is_lvalue_reference_v<LHS>
    && common_reference_with<const remove_reference_t<LHS>&, const remove_reference_t<RHS>&>
    && requires(LHS lhs, RHS&& rhs) {{ lhs = std::forward<RHS>(rhs) } -> same_as<LHS>; };

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_ASSIGNABLE_FROM_HPP_