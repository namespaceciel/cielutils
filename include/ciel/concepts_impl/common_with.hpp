#ifndef CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_COMMON_WITH_HPP_
#define CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_COMMON_WITH_HPP_

#include <ciel/concepts_impl/common_reference_with.hpp>
#include <ciel/concepts_impl/same_as.hpp>
#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

template<class T, class U>
concept common_with = same_as<common_type_t<T, U>, common_type_t<U, T>>
    && requires {
        static_cast<common_type_t<T, U>>(declval<T>());
        static_cast<common_type_t<T, U>>(declval<U>());
    }
    && common_reference_with<add_lvalue_reference_t<const T>,
                             add_lvalue_reference_t<const U>>
    && common_reference_with<add_lvalue_reference_t<common_type_t<T, U>>,
                             common_reference_t<add_lvalue_reference_t<const T>,
                                                add_lvalue_reference_t<const U>>>;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_COMMON_WITH_HPP_