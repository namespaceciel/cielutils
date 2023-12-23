#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_ENABLE_IF_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_ENABLE_IF_HPP_

#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

template<bool, class T = void>
struct enable_if {};

template<class T>
struct enable_if<true, T> {
    using type = T;
};

template<bool B, class T = void>
using enable_if_t = typename enable_if<B, T>::type;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_ENABLE_IF_HPP_