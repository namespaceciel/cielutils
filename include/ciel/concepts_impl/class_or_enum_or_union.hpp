#ifndef CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_CLASS_OR_ENUM_OR_UNION_HPP_
#define CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_CLASS_OR_ENUM_OR_UNION_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/is_class.hpp>
#include <ciel/type_traits_impl/is_enum.hpp>
#include <ciel/type_traits_impl/is_union.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
concept class_or_enum_or_union = is_class_v<T> || is_enum_v<T> || is_union_v<T>;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_CLASS_OR_ENUM_OR_UNION_HPP_