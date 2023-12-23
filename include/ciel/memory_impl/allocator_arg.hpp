#ifndef CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_ALLOCATOR_ARG_HPP_
#define CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_ALLOCATOR_ARG_HPP_

#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

struct allocator_arg_t { explicit allocator_arg_t() = default; };

inline constexpr allocator_arg_t allocator_arg{};

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_ALLOCATOR_ARG_HPP_