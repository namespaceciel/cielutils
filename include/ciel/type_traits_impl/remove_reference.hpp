#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_REMOVE_REFERENCE_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_REMOVE_REFERENCE_HPP_

#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
struct remove_reference {
    using type = T;
};

template<class T>
struct remove_reference<T&> {
    using type = T;
};

template<class T>
struct remove_reference<T&&> {
    using type = T;
};

template<class T>
using remove_reference_t = typename remove_reference<T>::type;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_REMOVE_REFERENCE_HPP_