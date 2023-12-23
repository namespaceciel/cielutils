#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_TYPE_IDENTITY_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_TYPE_IDENTITY_HPP_

#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
struct type_identity {
    using type = T;
};

template<class T>
using type_identity_t = typename type_identity<T>::type;

// It can be used to establish non-deduced contexts in template argument deduction
//
// template<class T>
// auto f(T, T) -> void;
//
// template<class T>
// auto g(T, type_identity_t<T>) -> void;
//
// f(4.2, 0); //  error
// g(4.2, 0); //  call g<double>

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_TYPE_IDENTITY_HPP_