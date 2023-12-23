#ifndef CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_COMMON_REFERENCE_WITH_HPP_
#define CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_COMMON_REFERENCE_WITH_HPP_

#include <ciel/concepts_impl/convertible_to.hpp>
#include <ciel/concepts_impl/same_as.hpp>
#include <ciel/config.hpp>
#include <ciel/type_traits_impl/common_reference.hpp>

NAMESPACE_CIEL_BEGIN

template<class T, class U>
concept common_reference_with = same_as<common_reference_t<T, U>, common_reference_t<U, T>>
    && convertible_to<T, common_reference_t<T, U>>
    && convertible_to<U, common_reference_t<T, U>>;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_COMMON_REFERENCE_WITH_HPP_