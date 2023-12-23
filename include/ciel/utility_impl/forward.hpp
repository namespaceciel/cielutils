#ifndef CIELUTILS_INCLUDE_CIEL_UTILITY_IMPL_FORWARD_HPP_
#define CIELUTILS_INCLUDE_CIEL_UTILITY_IMPL_FORWARD_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/remove_reference.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
[[deprecated("It's recommended to use std::forward")]]
constexpr auto forward(remove_reference_t<T>& t) noexcept -> T&& {
    return static_cast<T&&>(t);
}

template<class T>
[[deprecated("It's recommended to use std::forward")]]
constexpr auto forward(remove_reference_t<T>&& t) noexcept -> T&& {
    return static_cast<T&&>(t);
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_UTILITY_IMPL_FORWARD_HPP_