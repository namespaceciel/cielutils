#ifndef CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_DEFAULT_DELETE_HPP_
#define CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_DEFAULT_DELETE_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/is_convertible.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
struct default_delete {
    constexpr default_delete() noexcept = default;

    // serves for implicit convention from unique_ptr<Derived> to unique_ptr<Base>
    template<class U>
        requires is_convertible_v<U*, T*>
    default_delete(const default_delete<U>& /*unused*/) noexcept {}

    auto operator()(T* ptr) const -> void {
        delete ptr;
    }
};

template<class T>
struct default_delete<T[]> {
    constexpr default_delete() noexcept = default;

    template<class U>
        requires is_convertible_v<U(*)[], T(*)[]>
    default_delete(const default_delete<U[]>& /*unused*/) noexcept {}

    template<class U>
        requires is_convertible_v<U(*)[], T(*)[]>
    auto operator()(U* ptr) const -> void {
        delete[] ptr;
    }
};

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_DEFAULT_DELETE_HPP_