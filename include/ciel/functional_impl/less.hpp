#ifndef CIELUTILS_INCLUDE_CIEL_FUNCTIONAL_IMPL_LESS_HPP_
#define CIELUTILS_INCLUDE_CIEL_FUNCTIONAL_IMPL_LESS_HPP_

#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

template<class T = void>
struct less {
    [[nodiscard]] constexpr auto operator()(const T& lhs, const T& rhs) const -> bool {
        return lhs < rhs;
    }
};

template<>
struct less<void> {
    template<class T, class U>
    [[nodiscard]] constexpr auto operator()(T&& lhs, U&& rhs) const -> bool
        requires requires { std::forward<T>(lhs) < std::forward<U>(rhs); } {
        return std::forward<T>(lhs) < std::forward<U>(rhs);
    }

    using is_transparent = void;
};

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_FUNCTIONAL_IMPL_LESS_HPP_