#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_COMMON_REFERENCE_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_COMMON_REFERENCE_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/common_type.hpp>
#include <ciel/type_traits_impl/copy_cvref.hpp>
#include <ciel/type_traits_impl/is_convertible.hpp>
#include <ciel/type_traits_impl/remove_cvref.hpp>
#include <ciel/utility_impl/declval.hpp>

NAMESPACE_CIEL_BEGIN

// https://stackoverflow.com/questions/59011331/what-is-the-purpose-of-c20-stdcommon-reference

template<class, class, template<class> class, template<class> class>
struct basic_common_reference {};

namespace details {

template<class... T>
struct common_reference_helper {};

template<>
struct common_reference_helper<> {};

template<class T>
struct common_reference_helper<T> {
    using type = T;
};

template<class T1, class T2>
using simple_common_reference1 = decltype(true ? declval<copy_cv_t<T1, T2>>() : declval<copy_cv_t<T2, T1>>());

template<class T1, class T2, class X = remove_reference_t<T1>, class Y = remove_reference_t<T2>>
struct have_simple_common_reference_type {};

template<class T1, class T2, class X, class Y>
    requires requires { typename simple_common_reference1<X&, Y&>; } &&
        is_reference_v<simple_common_reference1<X&, Y&>>
struct have_simple_common_reference_type<T1&, T2&, X, Y> {
    using type = simple_common_reference1<X&, Y&>;
};

template<class T1, class T2, class X, class Y>
    requires requires { typename simple_common_reference1<X&&, Y&&>; } &&
        is_convertible_v<X&&, simple_common_reference1<X&&, Y&&>> &&
        is_convertible_v<Y&&, simple_common_reference1<X&&, Y&&>>
struct have_simple_common_reference_type<T1&&, T2&&, X, Y> {
    using type = simple_common_reference1<X&&, Y&&>;
};

template<class T1, class T2, class X, class Y>
    requires requires { typename simple_common_reference1<X&, const Y&>; } &&
        is_convertible_v<Y&&, simple_common_reference1<X&, const Y&>>
struct have_simple_common_reference_type<T1&, T2&&, X, Y> {
    using type = simple_common_reference1<X&, const Y&>;
};

template<class T1, class T2, class X, class Y>
struct have_simple_common_reference_type<T1&&, T2&, X, Y> : have_simple_common_reference_type<T2&, T1&&> {};

template<class T1, class T2>
using have_simple_common_reference_type_t = typename have_simple_common_reference_type<T1, T2>::type;

// if struct has type, the inherited type will be shadowed
template <class T1, class T2> struct common_reference_sub_bullet3;
template <class T1, class T2> struct common_reference_sub_bullet2 : common_reference_sub_bullet3<T1, T2> {};
template <class T1, class T2> struct common_reference_sub_bullet1 : common_reference_sub_bullet2<T1, T2> {};

template<class T1, class T2>
    requires is_reference_v<T1> && is_reference_v<T2> &&
        requires { typename have_simple_common_reference_type_t<T1, T2>; }
struct common_reference_sub_bullet1<T1, T2> {
    using type = have_simple_common_reference_type_t<T1, T2>;
};

template <class T>
struct TiQ {
    template<class U>
    using apply = copy_cvref_t<T, U>;
};

template <class T1, class T2>
using basic_common_reference_t = typename basic_common_reference<remove_cvref_t<T1>, remove_cvref_t<T2>,
                                        TiQ<T1>::template apply, TiQ<T2>::template apply>::type;

template<class T1, class T2>
    requires requires { typename basic_common_reference_t<T1, T2>; }
struct common_reference_sub_bullet2<T1, T2> {
    using type = basic_common_reference_t<T1, T2>;
};

template<class T>
auto val() -> T;

template<class T1, class T2>
using simple_common_reference3 = decltype(true ? val<T1>() : val<T2>());

template<class T1, class T2>
    requires requires { typename simple_common_reference3<T1, T2>; }
struct common_reference_sub_bullet3<T1, T2> {
    using type = simple_common_reference3<T1, T2>;
};

template<class T1, class T2>
struct common_reference_sub_bullet3 : common_type<T1, T2> {};

template<class T1, class T2>
struct common_reference_helper<T1, T2> : common_reference_sub_bullet1<T1, T2> {};

template <class T1, class T2, class... Rest>
    requires requires { typename common_reference_helper<T1, T2>::type; }
struct common_reference_helper<T1, T2, Rest...>
    : common_reference_helper<typename common_reference_helper<T1, T2>::type, Rest...> {};

}	// namespace details

template<class... T>
struct common_reference {
    using type = typename details::common_reference_helper<T...>::type;
};

template< class... T >
using common_reference_t = typename common_reference<T...>::type;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_COMMON_REFERENCE_HPP_