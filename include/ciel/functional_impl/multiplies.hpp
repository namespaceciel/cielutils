#ifndef CIELUTILS_INCLUDE_CIEL_FUNCTIONAL_IMPL_MULTIPLIES_HPP_
#define CIELUTILS_INCLUDE_CIEL_FUNCTIONAL_IMPL_MULTIPLIES_HPP_

#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

template<class T = void>
struct multiplies {
    [[nodiscard]] constexpr auto operator()(const T& lhs, const T& rhs) const -> T {
        return lhs * rhs;
    }
};

template<>
struct multiplies<void> {
    template<class T, class U>
    [[nodiscard]] constexpr auto operator()(T&& lhs, U&& rhs) const
        -> decltype(std::forward<T>(lhs) * std::forward<U>(rhs)) {
        return std::forward<T>(lhs) * std::forward<U>(rhs);
    }

    using is_transparent = void;
};

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_FUNCTIONAL_IMPL_MULTIPLIES_HPP_