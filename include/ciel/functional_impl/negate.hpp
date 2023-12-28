#ifndef CIELUTILS_INCLUDE_CIEL_FUNCTIONAL_IMPL_NEGATE_HPP_
#define CIELUTILS_INCLUDE_CIEL_FUNCTIONAL_IMPL_NEGATE_HPP_

#include <ciel/config.hpp>
#include <utility>

NAMESPACE_CIEL_BEGIN

template<class T = void>
struct negate {
    [[nodiscard]] constexpr auto operator()(const T& arg) const -> T {
        return -arg;
    }
};

template<>
struct negate<void> {
    template<class T>
    [[nodiscard]] constexpr auto operator()(T&& arg) const
        -> decltype(-std::forward<T>(arg)) {
        return -std::forward<T>(arg);
    }

    using is_transparent = void;
};

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_FUNCTIONAL_IMPL_NEGATE_HPP_