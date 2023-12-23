#ifndef CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_TO_ADDRESS_HPP_
#define CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_TO_ADDRESS_HPP_

#include <ciel/config.hpp>
#include <ciel/memory_impl/pointer_traits.hpp>
#include <ciel/type_traits_impl/is_function.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
[[nodiscard]] constexpr auto to_address(T* p) noexcept -> T* {
    static_assert(!is_function_v<T>);
    return p;
}

template<class T>
[[nodiscard]] constexpr auto to_address(const T& p) noexcept {
    if constexpr (requires{ pointer_traits<T>::to_address(p); }) {
        return pointer_traits<T>::to_address(p);
    } else {
        return ciel::to_address(p.operator->());
    }
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_TO_ADDRESS_HPP_