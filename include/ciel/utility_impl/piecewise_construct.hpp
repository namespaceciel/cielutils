#ifndef CIELUTILS_INCLUDE_CIEL_UTILITY_IMPL_PIECEWISE_CONSTRUCT_HPP_
#define CIELUTILS_INCLUDE_CIEL_UTILITY_IMPL_PIECEWISE_CONSTRUCT_HPP_

#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

struct piecewise_construct_t { explicit piecewise_construct_t() = default; };

inline constexpr piecewise_construct_t piecewise_construct{};

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_UTILITY_IMPL_PIECEWISE_CONSTRUCT_HPP_