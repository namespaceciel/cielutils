#ifndef CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_ALLOCATOR_TRAITS_HPP_
#define CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_ALLOCATOR_TRAITS_HPP_

#include <ciel/config.hpp>
#include <ciel/limits.hpp>
#include <ciel/memory_impl/construct_at.hpp>
#include <ciel/memory_impl/destroy_at.hpp>
#include <ciel/memory_impl/pointer_traits.hpp>
#include <ciel/type_traits_impl/is_empty.hpp>
#include <ciel/type_traits_impl/make_unsigned.hpp>

NAMESPACE_CIEL_BEGIN

namespace details {

template<class T, class, class = void>
struct has_pointer {
    using type = T*;
};

template<class T, class Alloc>
struct has_pointer<T, Alloc, void_t<typename remove_reference_t<Alloc>::pointer>> {
    using type = typename remove_reference_t<Alloc>::pointer;
};

template<class T, class Ptr, class, class = void>
struct has_const_pointer {
    using type = typename pointer_traits<Ptr>::template rebind<const T>;
};

template<class T, class Ptr, class Alloc>
struct has_const_pointer<T, Ptr, Alloc, void_t<typename remove_reference_t<Alloc>::const_pointer>> {
    using type = typename remove_reference_t<Alloc>::const_pointer;
};

template<class Ptr, class, class = void>
struct has_void_pointer {
    using type = typename pointer_traits<Ptr>::template rebind<void>;
};

template<class Ptr, class Alloc>
struct has_void_pointer<Ptr, Alloc, void_t<typename remove_reference_t<Alloc>::void_pointer>> {
    using type = typename remove_reference_t<Alloc>::void_pointer;
};

template<class Ptr, class, class = void>
struct has_const_void_pointer {
    using type = typename pointer_traits<Ptr>::template rebind<const void>;
};

template<class Ptr, class Alloc>
struct has_const_void_pointer<Ptr, Alloc, void_t<typename remove_reference_t<Alloc>::const_void_pointer>> {
    using type = typename remove_reference_t<Alloc>::const_void_pointer;
};

template<class Ptr, class, class = void>
struct has_difference_type {
    using type = typename pointer_traits<Ptr>::difference_type;
};

template<class Ptr, class Alloc>
struct has_difference_type<Ptr, Alloc, void_t<typename remove_reference_t<Alloc>::difference_type>> {
    using type = typename remove_reference_t<Alloc>::difference_type;
};

template<class Diff, class, class = void>
struct has_size_type : make_unsigned<Diff> {};

template<class Diff, class Alloc>
struct has_size_type<Diff, Alloc, void_t<typename remove_reference_t<Alloc>::size_type>> {
    using type = typename remove_reference_t<Alloc>::size_type;
};

template<class Alloc, class = void>
struct has_propagate_on_container_copy_assignment : false_type {};

template<class Alloc>
struct has_propagate_on_container_copy_assignment<Alloc,
    void_t<typename remove_reference_t<Alloc>::propagate_on_container_copy_assignment>> {
    using type = typename remove_reference_t<Alloc>::propagate_on_container_copy_assignment;
};

template<class Alloc, class = void>
struct has_propagate_on_container_move_assignment : false_type {};

template<class Alloc>
struct has_propagate_on_container_move_assignment<Alloc,
    void_t<typename remove_reference_t<Alloc>::propagate_on_container_move_assignment>> {
    using type = typename remove_reference_t<Alloc>::propagate_on_container_move_assignment;
};

template<class Alloc, class = void>
struct has_propagate_on_container_swap : false_type {};

template<class Alloc>
struct has_propagate_on_container_swap<Alloc, void_t<typename remove_reference_t<Alloc>::propagate_on_container_swap>> {
    using type = typename remove_reference_t<Alloc>::propagate_on_container_swap;
};

template<class Alloc, class = void>
struct has_is_always_equal : is_empty<Alloc> {};

template<class Alloc>
struct has_is_always_equal<Alloc, void_t<typename remove_reference_t<Alloc>::is_always_equal>> {
    using type = typename remove_reference_t<Alloc>::is_always_equal;
};

template<class T, class, class = void>
struct has_rebind_other : false_type {};

template<class T, class U>
struct has_rebind_other<T, U, void_t<typename T::template rebind<U>::other>> : true_type {};

template<class T, class U, bool = has_rebind_other<T, U>::value>
struct has_rebind_alloc {
    using type = typename T::template rebind<U>::other;
};

template<template<class, class...> class Alloc, class T, class... Args, class U>
struct has_rebind_alloc<Alloc<T, Args...>, U, true> {
    using type = typename Alloc<T, Args...>::template rebind<U>::other;
};

template<template<class, class...> class Alloc, class T, class... Args, class U>
struct has_rebind_alloc<Alloc<T, Args...>, U, false> {
    using type = Alloc<U, Args...>;
};

template<class, class, class, class = void>
struct has_allocate_hint : false_type {};

template<class Alloc, class SizeType, class ConstVoidPtr>
struct has_allocate_hint<Alloc, SizeType, ConstVoidPtr,
    decltype((void)declval<Alloc>().allocate(declval<SizeType>(), declval<ConstVoidPtr>()))> : true_type {};

template<class, class, class...>
struct has_construct : false_type {};

template<class Alloc, class... Args>
struct has_construct<decltype((void)declval<Alloc>().construct(declval<Args...>())), Alloc, Args...> : true_type {};

template<class, class, class = void>
struct has_destroy : false_type {};

template<class Alloc, class Ptr>
struct has_destroy<Alloc, Ptr, decltype((void)declval<Alloc>().destroy(declval<Ptr>()))> : true_type {};

template<class, class = void>
struct has_max_size : false_type {};

template<class Alloc>
struct has_max_size<Alloc, decltype((void)declval<Alloc>().max_size())> : true_type {};

template<class, class = void>
struct has_select_on_container_copy_construction : false_type {};

template<class Alloc>
struct has_select_on_container_copy_construction<Alloc,
    decltype((void)declval<Alloc>().select_on_container_copy_construction())> : true_type {};

}	// namespace details

template<class Alloc>
struct allocator_traits {

    using allocator_type                         = Alloc;
    using value_type                             = typename allocator_type::value_type;
    using pointer                                =
        typename details::has_pointer<value_type, allocator_type>::type;
    using const_pointer                          =
        typename details::has_const_pointer<value_type, pointer, allocator_type>::type;
    using void_pointer                           =
        typename details::has_void_pointer<pointer, allocator_type>::type;
    using const_void_pointer                     =
        typename details::has_const_void_pointer<pointer, allocator_type>::type;
    using difference_type                        =
        typename details::has_difference_type<pointer, allocator_type>::type;
    using size_type                              =
        typename details::has_size_type<difference_type, allocator_type>::type;
    using propagate_on_container_copy_assignment =
        typename details::has_propagate_on_container_copy_assignment<allocator_type>::type;
    using propagate_on_container_move_assignment =
        typename details::has_propagate_on_container_move_assignment<allocator_type>::type;
    using propagate_on_container_swap            =
        typename details::has_propagate_on_container_swap<allocator_type>::type;
    using is_always_equal                        =
        typename details::has_is_always_equal<allocator_type>::type;

    template<class U>
    using rebind_alloc                           = typename details::has_rebind_alloc<allocator_type, U>::type;
    template<class U>
    using rebind_traits                          = allocator_traits<rebind_alloc<U>>;

    [[nodiscard]] static constexpr auto allocate(allocator_type& a, size_type n) -> pointer {
        return a.allocate(n);
    }

    [[nodiscard]] static constexpr auto allocate(allocator_type& a, size_type n, const_void_pointer hint) -> pointer
        requires details::has_allocate_hint<allocator_type, size_type, const_void_pointer>::value {
        return a.allocate(n, hint);
    }

    [[nodiscard]] static constexpr auto allocate(allocator_type& a, size_type n, const_void_pointer /*unused*/)
        -> pointer
        requires (!details::has_allocate_hint<allocator_type, size_type, const_void_pointer>::value)  {
        return a.allocate(n);
    }

    static constexpr auto deallocate(allocator_type& a, pointer p, size_type n) -> void {
        a.deallocate(p, n);
    }

    template<class T, class... Args>
        requires details::has_construct<allocator_type, T*, Args...>::value
    static constexpr auto construct(allocator_type& a, T* p, Args&& ... args) -> void {
        a.construct(p, std::forward<Args>(args)...);
    }

    template<class T, class... Args>
        requires (!details::has_construct<allocator_type, T*, Args...>::value)
    static constexpr auto construct(allocator_type& /*unused*/, T* p, Args&& ... args) -> void {
        ciel::construct_at(p, std::forward<Args>(args)...);
    }

    template<class T>
        requires details::has_destroy<allocator_type, T*>::value
    static constexpr auto destroy(allocator_type& a, T* p) -> void {
        a.destroy(p);
    }

    template<class T>
        requires (!details::has_destroy<allocator_type, T*>::value)
    static constexpr auto destroy(allocator_type& /*unused*/, T* p) -> void {
        ciel::destroy_at(p);
    }

    [[nodiscard]] static constexpr auto max_size(const allocator_type& a) noexcept -> size_type
        requires (details::has_max_size<const allocator_type>::value) {
        return a.max_size();
    }

    [[nodiscard]] static constexpr auto max_size(const allocator_type& /*unused*/) noexcept -> size_type
        requires (!details::has_max_size<const allocator_type>::value) {
        return numeric_limits<size_type>::max() / sizeof(value_type);
    }

    [[nodiscard]] static constexpr auto select_on_container_copy_construction(const allocator_type& a) -> allocator_type
        requires (details::has_select_on_container_copy_construction<const allocator_type>::value) {
        return a.select_on_container_copy_construction();
    }

    [[nodiscard]] static constexpr auto select_on_container_copy_construction(const allocator_type& a) -> allocator_type
        requires (!details::has_select_on_container_copy_construction<const allocator_type>::value) {
        return a;
    }

};    // struct allocator_traits

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_ALLOCATOR_TRAITS_HPP_