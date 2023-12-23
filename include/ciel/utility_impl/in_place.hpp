#ifndef CIELUTILS_INCLUDE_CIEL_UTILITY_IMPL_IN_PLACE_HPP_
#define CIELUTILS_INCLUDE_CIEL_UTILITY_IMPL_IN_PLACE_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/integral_constant.hpp>
#include <ciel/type_traits_impl/remove_cvref.hpp>
#include <cstddef>

NAMESPACE_CIEL_BEGIN

struct in_place_t { explicit in_place_t() = default; };

inline constexpr in_place_t in_place{};

template<class T>
struct in_place_type_t { explicit in_place_type_t() = default; };

template<class T>
inline constexpr in_place_type_t<T> in_place_type{};

template<size_t I>
struct in_place_index_t { explicit in_place_index_t() = default; };

template<size_t I>
inline constexpr in_place_index_t<I> in_place_index{};

template<class T, class U = remove_cvref_t<T>>
struct is_inplace_type : false_type {};

template<class T, class In>
struct is_inplace_type<T, in_place_type_t<In>> : true_type {};

template<class T, class U = remove_cvref_t<T>>
struct is_inplace_index : false_type {};

template<class T, size_t I>
struct is_inplace_index<T, in_place_index_t<I>> : true_type {};

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_UTILITY_IMPL_IN_PLACE_HPP_