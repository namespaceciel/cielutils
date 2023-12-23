#ifndef CIELUTILS_INCLUDE_CIEL_UTILITY_IMPL_EXCHANGE_HPP_
#define CIELUTILS_INCLUDE_CIEL_UTILITY_IMPL_EXCHANGE_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/is_assignable.hpp>
#include <ciel/type_traits_impl/is_move_constructible.hpp>

NAMESPACE_CIEL_BEGIN

template<class T, class U = T>
constexpr auto exchange(T& obj, U&& new_value)
    noexcept(is_nothrow_move_constructible_v<T> && is_nothrow_assignable_v<T&, U>) -> T {
    T old_value = std::move(obj);
    obj = std::forward<U>(new_value);
    return old_value;
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_UTILITY_IMPL_EXCHANGE_HPP_