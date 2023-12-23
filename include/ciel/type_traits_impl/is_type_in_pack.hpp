#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_TYPE_IN_PACK_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_TYPE_IN_PACK_HPP_

#include <ciel/concepts_impl/same_as.hpp>
#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

template<class, class...>
struct is_type_in_pack;

template<class Target>
struct is_type_in_pack<Target> : false_type {};

template<class Target, class Front, class... Rest>
    requires same_as<Target, Front>
struct is_type_in_pack<Target, Front, Rest...> : true_type {};

template<class Target, class Front, class... Rest>
    requires (!same_as<Target, Front>)
struct is_type_in_pack<Target, Front, Rest...> : is_type_in_pack<Target, Rest...> {};

template<class Target, class... T>
inline constexpr bool is_type_in_pack_v = is_type_in_pack<Target, T...>::value;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_IS_TYPE_IN_PACK_HPP_