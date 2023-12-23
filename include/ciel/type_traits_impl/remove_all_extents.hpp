#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_REMOVE_ALL_EXTENTS_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_REMOVE_ALL_EXTENTS_HPP_

#include <ciel/config.hpp>
#include <cstddef>

NAMESPACE_CIEL_BEGIN

template<class T>
struct remove_all_extents {
    using type = T;
};

template<class T>
struct remove_all_extents<T[]> {
    using type = typename remove_all_extents<T>::type;
};

template<class T, size_t N>
struct remove_all_extents<T[N]> {
    using type = typename remove_all_extents<T>::type;
};

template<class T>
using remove_all_extents_t = typename remove_all_extents<T>::type;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_REMOVE_ALL_EXTENTS_HPP_