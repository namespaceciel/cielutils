#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_ADD_CV_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_ADD_CV_HPP_

#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
struct add_cv {
    using type = const volatile T;
};

template<class T>
struct add_const {
    using type = const T;
};

template<class T>
struct add_volatile {
    using type = volatile T;
};

template<class T>
using add_cv_t = typename add_cv<T>::type;

template<class T>
using add_const_t = typename add_const<T>::type;

template<class T>
using add_volatile_t = typename add_volatile<T>::type;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_ADD_CV_HPP_