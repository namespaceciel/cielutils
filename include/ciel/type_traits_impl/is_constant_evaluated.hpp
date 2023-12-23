#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_CONSTANT_EVALUATED_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_CONSTANT_EVALUATED_HPP_

#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

[[nodiscard]] constexpr auto is_constant_evaluated() noexcept -> bool {
    return __builtin_is_constant_evaluated();
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_CONSTANT_EVALUATED_HPP_