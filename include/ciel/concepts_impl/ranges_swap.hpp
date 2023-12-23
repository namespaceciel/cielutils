#ifndef CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_RANGES_SWAP_HPP_
#define CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_RANGES_SWAP_HPP_

#include <ciel/concepts_impl/assignable_from.hpp>
#include <ciel/concepts_impl/class_or_enum_or_union.hpp>
#include <ciel/concepts_impl/move_constructible.hpp>
#include <ciel/config.hpp>
#include <ciel/type_traits_impl/extent.hpp>
#include <ciel/type_traits_impl/is_move_assignable.hpp>
#include <ciel/utility_impl/exchange.hpp>

NAMESPACE_CIEL_BEGIN

namespace ranges {

// https://en.cppreference.com/w/cpp/utility/ranges/swap

namespace details {

template<class T>
auto swap(T &, T &) noexcept -> void = delete;

// case 1
// When T or U is class or enum or union, it may have its own swap function
template<class T, class U>
concept unqualified_swappable_with =
    (class_or_enum_or_union<remove_cvref_t<T>> || class_or_enum_or_union<remove_cvref_t<U>>)
    && requires(T &&t, U &&u) { swap(std::forward<T>(t), std::forward<U>(u)); };

struct _fn;

// case 2
template<class T, class U, size_t Size>
concept swappable_arrays = !unqualified_swappable_with<T(&)[Size], U(&)[Size]>
    && extent_v<T> == extent_v<U>
    && requires(T(&t)[Size], U(&u)[Size], const _fn &_swap){ _swap(t[0], u[0]); };

// case 3
template<class T>
concept exchangeable = !unqualified_swappable_with<T &, T &>
    && move_constructible<T>
    && assignable_from<T &, T>;

struct _fn {
    // case 1
    template<class T, class U>
    requires unqualified_swappable_with<T, U>
    constexpr auto operator()(T &&t, U &&u) const -> void {
        swap(std::forward<T>(t), std::forward<U>(u));
    }

    // case 2: may swap with different types
    template<class T, class U, size_t Size>
    requires swappable_arrays<T, U, Size>
    constexpr auto operator()(T(&t)[Size], U(&u)[Size]) const noexcept(noexcept((*this)(*t, *u))) -> void {
        for (size_t i = 0; i < Size; ++i) {
            (*this)(t[i], u[i]);
        }
    }

    // case 3
    template<exchangeable T>
    constexpr auto operator()(T &x, T &y) const noexcept(is_nothrow_move_constructible_v<T> &&
                                                         is_nothrow_move_assignable_v<T>) -> void {
        y = ciel::exchange(x, std::move(y));
    }
};

}   // namespace details

inline constexpr auto swap = details::_fn{};

}   // namespace ranges

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_RANGES_SWAP_HPP_