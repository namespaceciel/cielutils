#ifndef CIELUTILS_INCLUDE_CIEL_UTILITY_IMPL_MOVE_HPP_
#define CIELUTILS_INCLUDE_CIEL_UTILITY_IMPL_MOVE_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/remove_reference.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
[[deprecated("It's recommended to use std::move")]]
constexpr auto move(T&& t) noexcept -> remove_reference_t<T>&& {
    return static_cast<remove_reference_t<T>&&>(t);
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_UTILITY_IMPL_MOVE_HPP_