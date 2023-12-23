#ifndef CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_ADDRESSOF_HPP_
#define CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_ADDRESSOF_HPP_

#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
[[nodiscard]] constexpr auto addressof(T& arg) noexcept -> T* {
    return __builtin_addressof(arg);
}

template<class T>
auto addressof(const T&&) -> const T* = delete;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_ADDRESSOF_HPP_