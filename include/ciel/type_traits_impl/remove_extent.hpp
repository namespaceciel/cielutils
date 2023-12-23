#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_REMOVE_EXTENT_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_REMOVE_EXTENT_HPP_

#include <ciel/config.hpp>
#include <cstddef>

NAMESPACE_CIEL_BEGIN

template<class T>
struct remove_extent {
    using type = T;
};

template<class T>
struct remove_extent<T[]> {
    using type = T;
};

template<class T, size_t N>
struct remove_extent<T[N]> {
    using type = T;
};

template<class T>
using remove_extent_t = typename remove_extent<T>::type;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_REMOVE_EXTENT_HPP_