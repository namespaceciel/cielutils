#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_ALIGNED_UNION_H
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_ALIGNED_UNION_H

#include <ciel/algorithm_impl/max.hpp>
#include <ciel/config.hpp>
#include <cstddef>

NAMESPACE_CIEL_BEGIN

template<size_t Len, class... Types>
struct aligned_union {

    static constexpr size_t alignment_value = ciel::max({alignof(Types)...});

    struct type {
        alignas(alignment_value) char s[ciel::max({Len, sizeof(Types)...})];
    };
};

template<size_t Len, class... Types>
using aligned_union_t = typename aligned_union<Len,Types...>::type;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_ALIGNED_UNION_H