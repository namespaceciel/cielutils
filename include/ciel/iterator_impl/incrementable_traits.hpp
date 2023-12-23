#ifndef CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_INCREMENTABLE_TRAITS_HPP_
#define CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_INCREMENTABLE_TRAITS_HPP_

#include <ciel/concepts_impl/integral.hpp>
#include <ciel/config.hpp>
#include <ciel/type_traits_impl/is_object.hpp>
#include <ciel/type_traits_impl/make_signed.hpp>
#include <ciel/utility_impl/declval.hpp>
#include <cstddef>

NAMESPACE_CIEL_BEGIN

template<class I>
struct incrementable_traits {};

template<class T>
    requires is_object_v<T>
struct incrementable_traits<T*> {
    using difference_type = ptrdiff_t;
};

template<class T>
struct incrementable_traits<const T> : incrementable_traits<T> {};

template<class T>
    requires requires { typename T::difference_type; }
struct incrementable_traits<T> {
    using difference_type = typename T::difference_type;
};

template<class T>
    requires (!requires { typename T::difference_type; }) &&
        requires (const T& a, const T& b) { {a - b} -> integral; }
struct incrementable_traits<T>{
    using difference_type = make_signed_t<decltype(declval<T>() - declval<T>())>;
};

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_INCREMENTABLE_TRAITS_HPP_