#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_REMOVE_POINTER_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_REMOVE_POINTER_HPP_

#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
struct remove_pointer {
    using type = T;
};

template<class T>
struct remove_pointer<T*> {
    using type = T;
};

template<class T>
struct remove_pointer<T* const> {
    using type = T;
};

template<class T>
struct remove_pointer<T* volatile> {
    using type = T;
};

template<class T>
struct remove_pointer<T* const volatile> {
    using type = T;
};

template<class T>
using remove_pointer_t = typename remove_pointer<T>::type;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_REMOVE_POINTER_HPP_