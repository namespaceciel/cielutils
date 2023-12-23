#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_UNDERLYING_TYPE_H
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_UNDERLYING_TYPE_H

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/is_enum.hpp>

NAMESPACE_CIEL_BEGIN

namespace details {

template<class T, bool = is_enum_v<T>>
struct underlying_type_helper {};

template<class T>
struct underlying_type_helper<T, true> {
    using type = __underlying_type (T);
};

}   // namespace details

template<class T>
struct underlying_type : details::underlying_type_helper<T> {};

template<class T>
using underlying_type_t = typename underlying_type<T>::type;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_UNDERLYING_TYPE_H