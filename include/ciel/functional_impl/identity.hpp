#ifndef CIELUTILS_INCLUDE_CIEL_FUNCTIONAL_IMPL_IDENTITY_HPP_
#define CIELUTILS_INCLUDE_CIEL_FUNCTIONAL_IMPL_IDENTITY_HPP_

#include <ciel/config.hpp>
#include <utility>

NAMESPACE_CIEL_BEGIN

struct identity {
    template<class T>
    [[nodiscard]] constexpr auto operator()(T&& t) const noexcept -> T&& {
        return std::forward<T>(t);
    }

    using is_transparent = void;
};

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_FUNCTIONAL_IMPL_IDENTITY_HPP_