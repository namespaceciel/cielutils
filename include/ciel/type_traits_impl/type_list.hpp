#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_TYPE_LIST_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_TYPE_LIST_HPP_

#include <ciel/config.hpp>
#include <cstddef>

NAMESPACE_CIEL_BEGIN

template<class H, class T>
struct type_list {
    using head = H;
    using tail = T;
};

template<class TypeList, size_t size, bool = (size <= sizeof(typename TypeList::head))>
struct find_first;

template<class H, class T, size_t size>
struct find_first<type_list<H, T>, size, true> {
    using type = H;
};

template<class H, class T, size_t size>
struct find_first<type_list<H, T>, size, false> {
    using type = typename find_first<T, size>::type;
};

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_TYPE_LIST_HPP_