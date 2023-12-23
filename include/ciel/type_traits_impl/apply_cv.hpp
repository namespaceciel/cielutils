#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_APPLY_CV_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_APPLY_CV_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/is_const.hpp>
#include <ciel/type_traits_impl/is_volatile.hpp>
#include <ciel/type_traits_impl/remove_reference.hpp>

NAMESPACE_CIEL_BEGIN

template<class T, class U, bool = is_const_v<remove_reference_t<T>>, bool = is_volatile_v<remove_reference_t<T>>>
struct apply_cv {
    using type = U;
};

template<class T, class U>
struct apply_cv<T, U, true, true> {
    using type = const volatile U;
};

template<class T, class U>
struct apply_cv<T, U, true, false> {
    using type = const U;
};

template<class T, class U>
struct apply_cv<T, U, false, true> {
    using type = volatile U;
};

template<class T, class U>
struct apply_cv<T&, U, false, false> {
    using type = U&;
};

template<class T, class U>
struct apply_cv<T&, U, true, true> {
    using type = const volatile U&;
};

template<class T, class U>
struct apply_cv<T&, U, true, false> {
    using type = const U&;
};

template<class T, class U>
struct apply_cv<T&, U, false, true> {
    using type = volatile U&;
};

template<class T, class U>
using apply_cv_t = typename apply_cv<T, U>::type;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_APPLY_CV_HPP_