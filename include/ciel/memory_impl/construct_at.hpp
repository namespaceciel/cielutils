#ifndef CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_CONSTRUCT_AT_HPP_
#define CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_CONSTRUCT_AT_HPP_

#include <ciel/config.hpp>
#include <ciel/utility_impl/declval.hpp>

NAMESPACE_CIEL_BEGIN

template<class T, class... Args, class = decltype(::new(declval<void*>()) T(declval<Args>()...))>
constexpr auto construct_at(T* p, Args&& ... args) -> T* {
    // Convert p to void* to prevent unexpected operator new overloads
    return ::new(const_cast<void*>(static_cast<const volatile void*>(p))) T(std::forward<Args>(args)...);
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_CONSTRUCT_AT_HPP_