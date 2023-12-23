#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_ALIGNED_STORAGE_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_ALIGNED_STORAGE_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/alignment_of.hpp>
#include <ciel/type_traits_impl/is_move_constructible.hpp>
#include <cstddef>

NAMESPACE_CIEL_BEGIN

template<size_t Len, size_t Align>
struct aligned_storage {
    struct type {
        alignas(Align) unsigned char data[Len];
    };
};

template<size_t Len, size_t Align>
using aligned_storage_t = typename aligned_storage<Len, Align>::type;

using buffer_type = aligned_storage_t<sizeof(void*) * 3, alignment_of_v<void*>>;

template<class T>
using is_small_object = bool_constant<sizeof(T) <= sizeof(buffer_type) &&
                                            alignment_of_v<buffer_type> % alignment_of_v<T> == 0 &&
                                            is_nothrow_move_constructible_v<T>>;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_ALIGNED_STORAGE_HPP_