#ifndef CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_POINTER_TRAITS_HPP_
#define CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_POINTER_TRAITS_HPP_

#include <ciel/config.hpp>
#include <ciel/memory_impl/addressof.hpp>
#include <ciel/type_traits_impl/conditional.hpp>
#include <ciel/type_traits_impl/integral_constant.hpp>
#include <ciel/type_traits_impl/is_void.hpp>
#include <ciel/type_traits_impl/void_t.hpp>
#include <cstddef>

NAMESPACE_CIEL_BEGIN

namespace details {

template<class, class = void>
struct has_element_type : false_type {};

template<class T>
struct has_element_type<T, void_t<typename T::element_type>> : true_type {};

template<class Ptr, bool = has_element_type<Ptr>::value>
struct element_type_of {};

template<class Ptr>
struct element_type_of<Ptr, true> {
    using type = typename Ptr::element_type;
};

// Ptr = S<T, Args...>
template<template<class, class...> class S, class T, class... Args>
struct element_type_of<S<T, Args...>, true> {
    using type = typename S<T, Args...>::element_type;
};

template<template<class, class...> class S, class T, class... Args>
struct element_type_of<S<T, Args...>, false> {
    using type = T;
};

template<class, class = void>
struct pointer_has_difference_type : false_type {};

template<class T>
struct pointer_has_difference_type<T, void_t<typename T::difference_type>> : true_type {};

template<class Ptr, bool = pointer_has_difference_type<Ptr>::type>
struct difference_type_of {
    using type = ptrdiff_t;
};

template<class Ptr>
struct difference_type_of<Ptr, true> {
    using type = typename pointer_has_difference_type<Ptr>::type;
};

template<class T, class U>
struct has_rebind {
private:
    template<class>
    static auto test(...) -> false_type;
    template<class X>
    static auto test(typename X::template rebind<U>* = nullptr) -> true_type;

public:
    static const bool value = decltype(test<T>(nullptr))::value;
};

template<class T, class U, bool = has_rebind<T, U>::value>
struct rebind_of {
    using type = typename T::template rebind<U>;
};

template<template<class, class...> class S, class T, class... Args, class U>
struct rebind_of<S<T, Args...>, U, true> {
    using type = typename S<T, Args...>::template rebind<U>;
};

template<template<class, class...> class S, class T, class... Args, class U>
struct rebind_of<S<T, Args...>, U, false> {
    using type = S<T, Args...>;
};

}   // namespace details

template<class Ptr>
struct pointer_traits {

    // used in container iterator
    [[nodiscard]] static auto to_address(const Ptr& r) -> typename Ptr::pointer
        requires (requires (Ptr p) { p.base(); typename Ptr::pointer; }) {
        return r.base();
    }

};    // struct pointer_traits

template<class Ptr>
    requires requires { typename details::element_type_of<Ptr>::type; }
struct pointer_traits<Ptr> {

    using pointer         = Ptr;
    using element_type    = typename details::element_type_of<Ptr>::type;
    using difference_type = typename details::difference_type_of<Ptr>::type;

    template<class U>
    using rebind          = typename details::rebind_of<pointer, U>::type;

    [[nodiscard]] static auto pointer_to(conditional_t<is_void_v<element_type>, void, element_type&> r) -> pointer {
        return pointer::pointer_to(r);
    }

    // used in container iterator
    [[nodiscard]] static auto to_address(const Ptr& r) -> typename Ptr::pointer
        requires (requires (Ptr p) { p.base(); typename Ptr::pointer; }) {
        return r.base();
        }

};    // struct pointer_traits

template<class T>
struct pointer_traits<T*> {

    using pointer         = T*;
    using element_type    = T;
    using difference_type = ptrdiff_t;

    template<class U>
    using rebind          = U*;

    [[nodiscard]] static constexpr auto pointer_to(conditional_t<is_void_v<element_type>, void, element_type&> r)
        noexcept -> pointer {
        return ciel::addressof(r);
    }

};    // struct pointer_traits<T*>

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_POINTER_TRAITS_HPP_