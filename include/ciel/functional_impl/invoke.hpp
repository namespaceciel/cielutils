#ifndef CIELUTILS_INCLUDE_CIEL_FUNCTIONAL_IMPL_INVOKE_HPP_
#define CIELUTILS_INCLUDE_CIEL_FUNCTIONAL_IMPL_INVOKE_HPP_

#include <ciel/config.hpp>
#include <ciel/memory_impl/addressof.hpp>
#include <ciel/type_traits_impl/decay.hpp>
#include <ciel/type_traits_impl/is_base_of.hpp>
#include <ciel/type_traits_impl/is_member_pointer.hpp>
#include <ciel/type_traits_impl/is_same.hpp>
#include <ciel/type_traits_impl/is_void.hpp>
#include <ciel/utility_impl/declval.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
class reference_wrapper;

template<class T>
struct is_reference_wrapper : false_type {};

template<class U>
struct is_reference_wrapper<reference_wrapper<U>> : true_type {};

template<class F, class T, class Arg1, class T1 = remove_cvref_t<Arg1>, class... Args>
    requires (is_function_v<F> && (is_same_v<T, T1> || is_base_of_v<T, T1>))
auto INVOKE(F T::*f, Arg1&& t1, Args&& ... args)
    -> decltype((std::forward<Arg1>(t1).*f)(std::forward<Args>(args)...)) {
    return (std::forward<Arg1>(t1).*f)(std::forward<Args>(args)...);
}

template<class F, class T, class Arg1, class T1 = remove_cvref_t<Arg1>, class... Args>
    requires (is_function_v<F> && is_reference_wrapper<T1>::value)
auto INVOKE(F T::*f, Arg1&& t1, Args&& ... args)
    -> decltype((std::forward<Arg1>(t1).get().*f)(std::forward<Args>(args)...)) {
    return (std::forward<Arg1>(t1).get().*f)(std::forward<Args>(args)...);
}

template<class F, class T, class Arg1, class T1 = remove_cvref_t<Arg1>, class... Args>
    requires (is_function_v<F> && !(is_same_v<T, T1> || is_base_of_v<T, T1>) && !is_reference_wrapper<T1>::value)
auto INVOKE(F T::*f, Arg1&& t1, Args&& ... args)
    -> decltype(((*std::forward<Arg1>(t1)).*f)(std::forward<Args>(args)...)) {
    return ((*std::forward<Arg1>(t1)).*f)(std::forward<Args>(args)...);
}

template<class F, class T, class Arg1, class T1 = remove_cvref_t<Arg1>>
    requires (!is_function_v<F> && (is_same_v<T, T1> || is_base_of_v<T, T1>))
auto INVOKE(F T::*f, Arg1&& t1)
    -> decltype(std::forward<Arg1>(t1).*f) {
    return std::forward<Arg1>(t1).*f;
}

template<class F, class T, class Arg1, class T1 = remove_cvref_t<Arg1>>
    requires (!is_function_v<F> && is_reference_wrapper<T1>::value)
auto INVOKE(F T::*f, Arg1&& t1)
    -> decltype(std::forward<Arg1>(t1).get().*f) {
    return std::forward<Arg1>(t1).get().*f;
}

template<class F, class T, class Arg1, class T1 = remove_cvref_t<Arg1>>
    requires (!is_function_v<F> && !(is_same_v<T, T1> || is_base_of_v<T, T1>) && !is_reference_wrapper<T1>::value)
auto INVOKE(F T::*f, Arg1&& t1)
    -> decltype((*std::forward<Arg1>(t1)).*f) {
    return (*std::forward<Arg1>(t1)).*f;
}

template<class F, class... Args>
    requires (!is_member_pointer_v<F>)
auto INVOKE(F&& f, Args&& ... args)
    -> decltype(std::forward<F>(f)(std::forward<Args>(args)...)) {
    return std::forward<F>(f)(std::forward<Args>(args)...);
}

template<class R, class F, class... Args>
auto INVOKE(F&& f, Args&& ... args) -> R {
    if constexpr (is_void_v<R>) {
        static_cast<void>(INVOKE(std::forward<F>(f), std::forward<Args>(args)...));
    } else {
        return INVOKE(std::forward<F>(f), std::forward<Args>(args)...);
    }
}

template<class, class...>
struct invoke_result {};

template<class F, class... ArgTypes>
    requires requires { INVOKE(declval<F>(), declval<ArgTypes>()...); }
struct invoke_result<F, ArgTypes...> {
    using type = decltype(INVOKE(declval<F>(), declval<ArgTypes>()...));
};

template<class F, class... ArgTypes>
using invoke_result_t = typename invoke_result<F, ArgTypes...>::type;

template<class, class...>
struct is_invocable : false_type {};

template<class Fn, class... ArgTypes>
    requires requires { INVOKE(declval<Fn>(), declval<ArgTypes>()...); }
struct is_invocable<Fn, ArgTypes...> : true_type {};

template<class, class, class...>
struct is_invocable_r : false_type {};

template<class R, class Fn, class... ArgTypes>
    requires requires { INVOKE<R>(declval<Fn>(), declval<ArgTypes>()...); }
struct is_invocable_r<R, Fn, ArgTypes...> : true_type {};

template<class, class...>
struct is_nothrow_invocable : false_type {};

template<class Fn, class... ArgTypes>
    requires requires {{ INVOKE(declval<Fn>(), declval<ArgTypes>()...) } noexcept; }
struct is_nothrow_invocable<Fn, ArgTypes...> : true_type {};

template<class, class, class...>
struct is_nothrow_invocable_r : false_type {};

template<class R, class Fn, class... ArgTypes>
    requires requires {{ INVOKE<R>(declval<Fn>(), declval<ArgTypes>()...) } noexcept; }
struct is_nothrow_invocable_r<R, Fn, ArgTypes...> : true_type {};

template<class Fn, class... ArgTypes>
inline constexpr bool is_invocable_v = is_invocable<Fn, ArgTypes...>::value;

template<class R, class Fn, class... ArgTypes>
inline constexpr bool is_invocable_r_v = is_invocable_r<R, Fn, ArgTypes...>::value;

template<class Fn, class... ArgTypes>
inline constexpr bool is_nothrow_invocable_v = is_nothrow_invocable<Fn, ArgTypes...>::value;

template<class R, class Fn, class... ArgTypes>
inline constexpr bool is_nothrow_invocable_r_v = is_nothrow_invocable_r<R, Fn, ArgTypes...>::value;

template<class F, class... Args>
    requires is_invocable_v<F, Args...>
constexpr auto invoke(F&& f, Args&& ... args) noexcept(is_nothrow_invocable_v<F, Args...>)
    -> invoke_result_t<F, Args...> {
    return INVOKE(std::forward<F>(f), std::forward<Args>(args)...);
}

template<class R, class F, class... Args>
    requires is_invocable_r_v<R, F, Args...>
constexpr auto invoke_r(F && f, Args && ...args) noexcept(is_nothrow_invocable_r_v<R, F, Args...>) -> R {
    return INVOKE<R>(std::forward<F>(f), std::forward<Args>(args)...);
}

namespace details {

template<class T>
auto FUN(T& t) noexcept -> T& {
    return t;
}

template<class T>
auto FUN(T&&) -> void = delete;

}   // namespace details

template<class T>
class reference_wrapper {
public:
    using type = T;

    template<class U>
        requires (!is_same_v<decay_t<U>, reference_wrapper>) && requires { details::FUN(declval<U>()); }
    constexpr reference_wrapper(U&& u) noexcept(noexcept(details::FUN(declval<U>())))
        : ptr(ciel::addressof(details::FUN<T>(std::forward<U>(u)))) {}

    [[nodiscard]] constexpr operator T&() const noexcept {
        return *ptr;
    }

    [[nodiscard]] constexpr auto get() const noexcept -> T& {
        return *ptr;
    }

    template<class... ArgTypes>
    constexpr auto operator()(ArgTypes&& ... args) const noexcept(is_nothrow_invocable_v<T&, ArgTypes...>)
        -> invoke_result_t<T&, ArgTypes...> {
        return invoke(get(), std::forward<ArgTypes>(args)...);
    }

private:
    T* ptr;
};

template<class T>
reference_wrapper(T&) -> reference_wrapper<T>;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_FUNCTIONAL_IMPL_INVOKE_HPP_