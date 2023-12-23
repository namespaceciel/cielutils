#ifndef CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_DESTROY_AT_HPP_
#define CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_DESTROY_AT_HPP_

#include <ciel/config.hpp>
#include <ciel/memory_impl/addressof.hpp>
#include <ciel/type_traits_impl/is_array.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
constexpr auto destroy_at(T* p) -> void {
    if constexpr (is_array_v<T>) {
        for (auto& elem : *p) {
            destroy_at(ciel::addressof(elem));
        }
    } else {
        p->~T();
    }
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_DESTROY_AT_HPP_