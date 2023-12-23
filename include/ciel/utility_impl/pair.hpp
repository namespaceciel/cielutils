#ifndef CIELUTILS_INCLUDE_CIEL_UTILITY_IMPL_PAIR_HPP_
#define CIELUTILS_INCLUDE_CIEL_UTILITY_IMPL_PAIR_HPP_

#include <ciel/config.hpp>
#include <ciel/synth_three_way.hpp>
#include <ciel/type_traits_impl/decay.hpp>
#include <ciel/type_traits_impl/is_copy_assignable.hpp>
#include <ciel/type_traits_impl/is_copy_constructible.hpp>
#include <ciel/type_traits_impl/is_implicitly_default_constructible.hpp>
#include <ciel/type_traits_impl/is_swappable.hpp>
#include <ciel/utility_impl/integer_sequence.hpp>
#include <ciel/utility_impl/piecewise_construct.hpp>

NAMESPACE_CIEL_BEGIN

template<class...>
struct tuple;
template<class, class>
struct pair;

NAMESPACE_CIEL_END

namespace std {

template<size_t I, class... Types>
constexpr auto get(ciel::tuple<Types...>& t) noexcept -> tuple_element_t<I, ciel::tuple<Types...>>&;
template<size_t I, class... Types>
constexpr auto get(ciel::tuple<Types...>&& t) noexcept -> tuple_element_t<I, ciel::tuple<Types...>>&&;
template<size_t I, class... Types>
constexpr auto get(const ciel::tuple<Types...>& t) noexcept -> tuple_element_t<I, ciel::tuple<Types...>> const&;
template<size_t I, class... Types>
constexpr auto get(const ciel::tuple<Types...>&& t) noexcept -> tuple_element_t<I, ciel::tuple<Types...>> const&&;

template<size_t I, class T1, class T2>
constexpr auto get(ciel::pair<T1, T2>& p) noexcept -> tuple_element_t<I, ciel::pair<T1,T2>>&;
template<size_t I, class T1, class T2>
constexpr auto get(const ciel::pair<T1,T2>& p) noexcept -> tuple_element_t<I, ciel::pair<T1,T2>> const&;
template<size_t I, class T1, class T2>
constexpr auto get(ciel::pair<T1,T2>&& p) noexcept -> tuple_element_t<I, ciel::pair<T1,T2>>&&;
template<size_t I, class T1, class T2>
constexpr auto get(const ciel::pair<T1,T2>&& p) noexcept -> tuple_element_t<I, ciel::pair<T1,T2>> const&&;

}   // namespace std

NAMESPACE_CIEL_BEGIN

template<class T1, class T2>
struct pair {
public:
    using first_type  = T1;
    using second_type = T2;

    T1 first;
    T2 second;

    constexpr explicit(!is_implicitly_default_constructible_v<T1> || !is_implicitly_default_constructible_v<T2>)
    pair() requires (is_default_constructible_v<T1> && is_default_constructible_v<T2>) : first(), second() {}

    constexpr explicit(!is_convertible_v<const T1&, T1> || !is_convertible_v<const T2&, T2>)
    pair(const T1& x, const T2& y) requires (is_copy_constructible_v<T1> && is_copy_constructible_v<T2>)
        : first(x), second(y) {}

    template<class U1, class U2>
        requires (is_constructible_v<T1, U1> && is_constructible_v<T2, U2>)
    constexpr explicit(!is_convertible_v<U1, T1> || !is_convertible_v<U2, T2>)
    pair(U1&& x, U2&& y)
        : first(std::forward<U1>(x)), second(std::forward<U2>(y)) {}

    template<class U1, class U2>
        requires (is_constructible_v<T1, const U1&> && is_constructible_v<T2, const U2&>)
    constexpr explicit(!is_convertible_v<const U1&, T1> || !is_convertible_v<const U2&, T2>)
    pair(const pair<U1, U2>& p)
        : first(p.first), second(p.second) {}

    template<class U1, class U2>
        requires (is_constructible_v<T1, U1> && is_constructible_v<T2, U2>)
    constexpr explicit(!is_convertible_v<U1, T1> || !is_convertible_v<U2, T2>)
    pair(pair<U1, U2>&& p)
        : first(std::forward<U1>(p.first)), second(std::forward<U2>(p.second)) {}

    template<class... Args1, class... Args2>
    constexpr pair(piecewise_construct_t pc, tuple<Args1...>&& first_args, tuple<Args2...>&& second_args)
        : pair(pc, std::move(first_args), std::move(second_args),
               index_sequence_for<Args1...>(), index_sequence_for<Args2...>()) {}

private:
    template<class... Args1, class... Args2, size_t... I1, size_t... I2>
    constexpr pair(piecewise_construct_t /*unused*/, tuple<Args1...>&& first_args, tuple<Args2...>&& second_args,
                   index_sequence<I1...> /*unused*/, index_sequence<I2...> /*unused*/)
        : first(std::forward<Args1>(std::get<I1>(first_args))...),
        second(std::forward<Args2>(std::get<I2>(second_args))...) {}

public:
    constexpr pair(const pair& p) = default;

    constexpr pair(pair&& p) noexcept = default;

    constexpr ~pair() = default;

    constexpr auto operator=(const pair& other) -> pair&
        requires is_copy_assignable_v<T1> && is_copy_assignable_v<T2> {
        first = other.first;
        second = other.second;
        return *this;
    }

    template<class U1, class U2>
    constexpr auto operator=(const pair<U1, U2>& other) -> pair&
        requires is_assignable_v<T1&, const U1&> && is_assignable_v<T2&, const U2&> {
        first = other.first;
        second = other.second;
        return *this;
    }

    constexpr auto operator=(pair&& other)
        noexcept(is_nothrow_move_assignable_v<T1> && is_nothrow_move_assignable_v<T2>) -> pair&
        requires is_move_assignable_v<T1> && is_move_assignable_v<T2> {
        first = std::move(other.first);
        second = std::move(other.second);
        return *this;
    }

    template<class U1, class U2>
    constexpr auto operator=(pair<U1, U2>&& other) -> pair&
        requires is_assignable_v<T1&, U1> && is_assignable_v<T2&, U2> {
        first = std::forward<U1>(other.first);
        second = std::forward<U2>(other.second);
        return *this;
    }

    constexpr auto swap(pair& other) noexcept(is_nothrow_swappable_v<T1> && is_nothrow_swappable_v<T2>) -> void {
        using std::swap;
        swap(first, other.first);
        swap(second, other.second);
    }

    // The following serve for structure binding
    template<size_t I>
    [[nodiscard]] constexpr auto&& get() & noexcept {
        return std::get<I>(*this);
    }

    template<size_t I>
    [[nodiscard]] constexpr auto&& get() const& noexcept {
        return std::get<I>(*this);
    }

    template<size_t I>
    [[nodiscard]] constexpr auto&& get() && noexcept {
        return std::get<I>(std::move(*this));
    }

    template<size_t I>
    [[nodiscard]] constexpr auto&& get() const&& noexcept {
        return std::get<I>(std::move(*this));
    }

};    // struct pair

template<class T>
struct unwrap_ref {
    using type = T;
};

template<class>
class reference_wrapper;

template<class T>
struct unwrap_ref<reference_wrapper<T>> {
    using type = T&;
};

template<class T1, class T2, class V1 = typename unwrap_ref<decay_t<T1>>::type,
                             class V2 = typename unwrap_ref<decay_t<T2>>::type>
[[nodiscard]] constexpr auto make_pair(T1&& t, T2&& u) -> pair<V1, V2> {
    return pair<V1, V2>(std::forward<T1>(t), std::forward<T2>(u));
}

template<class T1, class T2, class U1, class U2>
[[nodiscard]] constexpr auto operator==(const pair<T1, T2>& lhs, const pair<U1, U2>& rhs) -> bool {
    return lhs.first == rhs.first && lhs.second == rhs.second;
}

template<class T1, class T2, class U1, class U2>
[[nodiscard]] constexpr auto operator<=>(const pair<T1, T2>& lhs, const pair<U1, U2>& rhs)
    -> std::common_comparison_category_t<synth_three_way_result<T1, U1>, synth_three_way_result<T2, U2>> {
    if (auto stw = ciel::synth_three_way(lhs.first, rhs.first); stw != 0) {
        return stw;
    }
    return ciel::synth_three_way(lhs.second, rhs.second);
}

template<class T1, class T2>
pair(T1, T2) -> pair<T1, T2>;

NAMESPACE_CIEL_END

namespace std {

template<class T1, class T2>
constexpr auto swap(ciel::pair<T1, T2>& x, ciel::pair<T1, T2>& y) noexcept(noexcept(x.swap(y))) -> void
requires (is_swappable_v<typename ciel::pair<T1, T2>::first_type> &&
          is_swappable_v<typename ciel::pair<T1, T2>::second_type>) {
    x.swap(y);
}

template<size_t I, class T1, class T2>
[[nodiscard]] constexpr auto get(ciel::pair<T1, T2>& p) noexcept
    -> tuple_element_t<I, ciel::pair<T1,T2>>& {
    if constexpr (I == 0) {
        return p.first;
    } else {
        return p.second;
    }
}

template<size_t I, class T1, class T2>
[[nodiscard]] constexpr auto get(const ciel::pair<T1,T2>& p) noexcept
    -> tuple_element_t<I, ciel::pair<T1,T2>> const& {
    if constexpr (I == 0) {
        return p.first;
    } else {
        return p.second;
    }
}

template<size_t I, class T1, class T2>
[[nodiscard]] constexpr auto get(ciel::pair<T1,T2>&& p) noexcept
    -> tuple_element_t<I, ciel::pair<T1,T2>>&& {
    if constexpr (I == 0) {
        return std::move(p.first);
    } else {
        return std::move(p.second);
    }
}

template<size_t I, class T1, class T2>
[[nodiscard]] constexpr auto get(const ciel::pair<T1,T2>&& p) noexcept
    -> tuple_element_t<I, ciel::pair<T1,T2>> const&& {
    if constexpr (I == 0) {
        return std::move(p.first);
    } else {
        return std::move(p.second);
    }
}

template<class T, class U>
[[nodiscard]] constexpr auto get(ciel::pair<T, U>& p) noexcept -> T& {
    return p.first;
}

template<class T, class U>
[[nodiscard]] constexpr auto get(const ciel::pair<T, U>& p) noexcept -> const T& {
    return p.first;
}

template<class T, class U>
[[nodiscard]] constexpr auto get(ciel::pair<T, U>&& p) noexcept -> T&& {
    return std::move(p.first);
}

template<class T, class U>
[[nodiscard]] constexpr auto get(const ciel::pair<T, U>&& p) noexcept -> const T&& {
    return std::move(p.first);
}

template<class T, class U>
[[nodiscard]] constexpr auto get(ciel::pair<U, T>& p) noexcept -> T& {
    return p.second;
}

template<class T, class U>
[[nodiscard]] constexpr auto get(const ciel::pair<U, T>& p) noexcept -> const T& {
    return p.second;
}

template<class T, class U>
[[nodiscard]] constexpr auto get(ciel::pair<U, T>&& p) noexcept -> T&& {
    return std::move(p.second);
}

template<class T, class U>
[[nodiscard]] constexpr auto get(const ciel::pair<U, T>&& p) noexcept -> const T&& {
    return std::move(p.second);
}

template<class T1, class T2>
struct tuple_size<ciel::pair<T1, T2>> : integral_constant<size_t, 2> {};

template<class T1, class T2>
struct tuple_element<0, ciel::pair<T1, T2>> {
    using type = T1;
};

template<class T1, class T2>
struct tuple_element<1, ciel::pair<T1, T2>> {
    using type = T2;
};

}   // namespace std

#endif // CIELUTILS_INCLUDE_CIEL_UTILITY_IMPL_PAIR_HPP_