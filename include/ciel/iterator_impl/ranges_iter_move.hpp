#ifndef CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_RANGES_ITER_MOVE_HPP_
#define CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_RANGES_ITER_MOVE_HPP_

#include <ciel/concepts_impl/class_or_enum_or_union.hpp>
#include <ciel/config.hpp>
#include <ciel/type_traits_impl/is_lvalue_reference.hpp>
#include <ciel/type_traits_impl/is_rvalue_reference.hpp>
#include <ciel/type_traits_impl/remove_cvref.hpp>
#include <ciel/utility_impl/declval.hpp>

NAMESPACE_CIEL_BEGIN

namespace ranges {

namespace details {

auto iter_move() -> void;

template<class T>
concept unqualified_iter_move = class_or_enum_or_union<remove_cvref_t<T>>
                                && requires { iter_move(declval<T>()); };

template<class T>
concept can_move = !unqualified_iter_move<T>
                   && requires{
    *declval<T>();
    requires is_lvalue_reference_v<decltype(*declval<T>())>;
};

template<class T>
concept can_deref = !unqualified_iter_move<T>
                    && !can_move<T>
                    && requires{
    *declval<T>();
    requires is_rvalue_reference_v<decltype(*declval<T>())>;
};

struct fn {
    // case 1
    template<class T>
    requires unqualified_iter_move<T>
    constexpr auto operator()(T &&t) const noexcept(noexcept(iter_move(std::forward<T>(t)))) -> void {
        return iter_move(std::forward<T>(t));
    }

    // case 2
    template<class T>
    requires can_move<T>
    constexpr auto
    operator()(T &&t) const noexcept(noexcept(std::move(*std::forward<T>(t)))) -> decltype(std::move(
            *std::forward<T>(t))) {
        return std::move(*std::forward<T>(t));
    }

    // case 3
    template<class T>
    requires can_deref<T>
    constexpr auto
    operator()(T &&t) const noexcept(noexcept(*std::forward<T>(t))) -> decltype(*std::forward<T>(t)) {
        return *std::forward<T>(t);
    }
};

}   // namespace details

inline constexpr auto iter_move = details::fn{};

}   // namespace ranges

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_RANGES_ITER_MOVE_HPP_