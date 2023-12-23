#ifndef CIELUTILS_INCLUDE_CIEL_FUNCTIONAL_IMPL_STRIP_SIGNATURE_H
#define CIELUTILS_INCLUDE_CIEL_FUNCTIONAL_IMPL_STRIP_SIGNATURE_H

#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

template<class>
struct strip_signature;

template<class R, class Class, class... Args>
struct strip_signature<R (Class::*)(Args...)> {
    using type = R(Args...);
};

template<class R, class Class, class... Args>
struct strip_signature<R (Class::*)(Args...) const> {
    using type = R(Args...);
};

template<class R, class Class, class... Args>
struct strip_signature<R (Class::*)(Args...) volatile> {
    using type = R(Args...);
};

template<class R, class Class, class... Args>
struct strip_signature<R (Class::*)(Args...) const volatile> {
    using type = R(Args...);
};

template<class R, class Class, class... Args>
struct strip_signature<R (Class::*)(Args...)&> {
    using type = R(Args...);
};

template<class R, class Class, class... Args>
struct strip_signature<R (Class::*)(Args...) const&> {
    using type = R(Args...);
};

template<class R, class Class, class... Args>
struct strip_signature<R (Class::*)(Args...) volatile&> {
    using type = R(Args...);
};

template<class R, class Class, class... Args>
struct strip_signature<R (Class::*)(Args...) const volatile&> {
    using type = R(Args...);
};

template<class R, class Class, class... Args>
struct strip_signature<R (Class::*)(Args...) noexcept> {
    using type = R(Args...);
};

template<class R, class Class, class... Args>
struct strip_signature<R (Class::*)(Args...) const noexcept> {
    using type = R(Args...);
};

template<class R, class Class, class... Args>
struct strip_signature<R (Class::*)(Args...) volatile noexcept> {
    using type = R(Args...);
};

template<class R, class Class, class... Args>
struct strip_signature<R (Class::*)(Args...) const volatile noexcept> {
    using type = R(Args...);
};

template<class R, class Class, class... Args>
struct strip_signature<R (Class::*)(Args...)& noexcept> {
    using type = R(Args...);
};

template<class R, class Class, class... Args>
struct strip_signature<R (Class::*)(Args...) const& noexcept> {
    using type = R(Args...);
};

template<class R, class Class, class... Args>
struct strip_signature<R (Class::*)(Args...) volatile& noexcept> {
    using type = R(Args...);
};

template<class R, class Class, class... Args>
struct strip_signature<R (Class::*)(Args...) const volatile& noexcept> {
    using type = R(Args...);
};

template<class F>
using strip_signature_t = typename strip_signature<F>::type;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_FUNCTIONAL_IMPL_STRIP_SIGNATURE_H