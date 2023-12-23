#ifndef CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_USES_ALLOCATOR_HPP_
#define CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_USES_ALLOCATOR_HPP_

#include <ciel/config.hpp>
#include <ciel/memory_impl/allocator_arg.hpp>
#include <ciel/type_traits_impl/is_constructible.hpp>
#include <ciel/type_traits_impl/is_convertible.hpp>
#include <ciel/type_traits_impl/remove_cvref.hpp>

NAMESPACE_CIEL_BEGIN

template<class, class>
struct uses_allocator : false_type {};

template<class T, class Alloc>
    requires requires { typename T::allocator_type; } && is_convertible_v<Alloc, typename T::allocator_type>
struct uses_allocator<T, Alloc> : true_type {};

template<class T, class Alloc>
inline constexpr bool uses_allocator_v = uses_allocator<T, Alloc>::value;

template<class T, class Alloc, class... Args>
struct uses_alloc_ctor_impl {
    static const bool ua = uses_allocator_v<T, remove_cvref_t<Alloc>>;
    static const bool ic = is_constructible_v<T, allocator_arg_t, Alloc, Args...>;
    static const int value = ua ? 2 - ic : 0;
};

template<class T, class Alloc, class ...Args>
struct uses_alloc_ctor : integral_constant<int, uses_alloc_ctor_impl<T, Alloc, Args...>::value> {};

template<class T, class Alloc, class... Args>
auto user_alloc_construct_impl(integral_constant<int, 0> /*unused*/, T* storage, const Alloc& /*unused*/,
                               Args&& ... args) -> void {
    new(storage) T(std::forward<Args>(args)...);
}

template<class T, class Alloc, class... Args>
auto user_alloc_construct_impl(integral_constant<int, 1> /*unused*/, T* storage, Alloc& a, Args&& ... args) -> void {
    new(storage) T(allocator_arg, a, std::forward<Args>(args)...);
}

template<class T, class Alloc, class... Args>
auto user_alloc_construct_impl(integral_constant<int, 2> /*unused*/, T* storage, Alloc& a, Args&& ... args) -> void {
    new(storage) T(std::forward<Args>(args)..., a);
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_USES_ALLOCATOR_HPP_