#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_COMMON_TYPE_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_COMMON_TYPE_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/decay.hpp>
#include <ciel/type_traits_impl/is_same.hpp>
#include <ciel/utility_impl/declval.hpp>

NAMESPACE_CIEL_BEGIN

template<class... T>
struct common_type;

namespace details {

template<class T1, class T2>
using common_type_alias = decltype(true ? declval<T1>() : declval<T2>());

template<class...>
struct common_type_helper {};

template<>
struct common_type_helper<> {};

template<class T>
struct common_type_helper<T> : common_type<T, T> {};

// if struct has type, the inherited type will be shadowed
template<class T1, class T2> struct common_type_sub_bullet4 {};
template<class T1, class T2> struct common_type_sub_bullet3 : common_type_sub_bullet4<T1, T2> {};
template<class T1, class T2> struct common_type_sub_bullet2 : common_type_sub_bullet3<T1, T2> {};
template<class T1, class T2> struct common_type_sub_bullet1 : common_type_sub_bullet2<T1, T2> {};

template<class T1, class T2>
    requires (!is_same_v<T1, decay_t<T1>> || !is_same_v<T2, decay_t<T2>>) &&
        requires { typename common_type<decay_t<T1>, decay_t<T2>>::type; }
struct common_type_sub_bullet1<T1, T2> {
    using type = typename common_type<decay_t<T1>, decay_t<T2>>::type;
};

template<class T1, class T2>
    requires requires { typename common_type<T1, T2>::type; }
struct common_type_sub_bullet2<T1, T2> {
    using type = typename common_type<T1, T2>::type;
};

template<class T1, class T2>
    requires requires { typename decay_t<common_type_alias<T1, T2>>; }
struct common_type_sub_bullet3<T1, T2> {
    using type = decay_t<common_type_alias<T1, T2>>;
};

template<class T1, class T2>
    requires requires { typename decay_t<common_type_alias<const remove_reference_t<T1>&,
            const remove_reference_t<T2>&>>; }
struct common_type_sub_bullet4<T1, T2> {
    using type = decay_t<common_type_alias<const remove_reference_t<T1>&, const remove_reference_t<T2>&>>;
};

template<class T1, class T2>
struct common_type_helper<T1, T2> : common_type_sub_bullet1<T1 ,T2> {};

template<class T1, class T2, class... Rest>
struct common_type_helper<T1, T2, Rest...>
    : common_type_helper<typename common_type_helper<T1, T2>::type, Rest...> {};

}	// namespace details

template<class... T>
struct common_type : details::common_type_helper<T...> {};

template<class... T>
using common_type_t = typename common_type<T...>::type;

// Line 26: Why straightly deriving from common_type_helper<T, T> will get compile error
//     when common_type<T, T> is literally derived from common_type_helper<T, T>?

// e.g.
// common_type_t<int>
//
// When deriving from common_type<T, T>, the instantiating order is:
// common_type<int>
// common_type_helper<int>
// *common_type<int, int>
// common_type_helper<int, int>
// common_type_sub_bullet1<int, int>
// common_type_sub_bullet2<int, int>   // Line 42 checks if common_type<int, int> has type
// common_type_sub_bullet3<int, int>
// common_type_sub_bullet4<int, int>
//
// So at Line 42 common_type<int, int> is already existed and doesn't have type
//
// But when straightly deriving from common_type_helper<T, T>, common_type<int, int> is never instantiated instead,
// at this time, line 42 just begins to instantiate, and it will derive from common_type_helper<int, int>, which is
// an incomplete type, so it yields an error

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_COMMON_TYPE_HPP_