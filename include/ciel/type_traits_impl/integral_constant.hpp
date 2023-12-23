#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_INTEGRAL_CONSTANT_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_INTEGRAL_CONSTANT_HPP_

#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

template<class T, T v>
struct integral_constant {
    static constexpr T value = v;
    using value_type = T;
    using type       = integral_constant;

    constexpr operator value_type() const noexcept {
        return value;
    }
    constexpr auto operator()() const noexcept -> value_type {
        return value;
    }
};

template<bool B>
using bool_constant = integral_constant<bool, B>;

using true_type     = bool_constant<true>;
using false_type    = bool_constant<false>;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_INTEGRAL_CONSTANT_HPP_