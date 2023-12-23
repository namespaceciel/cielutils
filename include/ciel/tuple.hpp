#ifndef CIELUTILS_INCLUDE_CIEL_TUPLE_HPP_
#define CIELUTILS_INCLUDE_CIEL_TUPLE_HPP_

#include <ciel/config.hpp>
#include <ciel/memory_impl/addressof.hpp>
#include <ciel/memory_impl/allocator_arg.hpp>
#include <ciel/memory_impl/uses_allocator.hpp>
#include <ciel/synth_three_way.hpp>
#include <ciel/type_traits_impl/copy_cvref.hpp>
#include <ciel/type_traits_impl/find_index_in_args.hpp>
#include <ciel/type_traits_impl/is_convertible.hpp>
#include <ciel/type_traits_impl/is_copy_constructible.hpp>
#include <ciel/type_traits_impl/is_empty.hpp>
#include <ciel/type_traits_impl/is_final.hpp>
#include <ciel/type_traits_impl/is_implicitly_default_constructible.hpp>
#include <ciel/type_traits_impl/remove_cvref.hpp>
#include <ciel/utility_impl/integer_sequence.hpp>
#include <ciel/utility_impl/pair.hpp>

NAMESPACE_CIEL_BEGIN

template<class...>
struct tuple;

namespace details {

template<size_t, class>
struct tuple_leaf;
template<class, class...>
struct tuple_impl;

}   // namespace details

NAMESPACE_CIEL_END

namespace std {

template<class... Types>
struct tuple_size<ciel::tuple<Types...>> : ciel::integral_constant<size_t, sizeof...(Types)> {};

template<size_t I, class Head, class... Tail>
struct tuple_element<I, ciel::tuple<Head, Tail...>> : tuple_element<I - 1, ciel::tuple<Tail...>> {};

template<class Head, class... Tail>
struct tuple_element<0, ciel::tuple<Head, Tail...>> {
    using type = Head;
};

template<size_t I, class... Types>
[[nodiscard]] constexpr auto get(ciel::tuple<Types...>& t) noexcept
    -> tuple_element_t<I, ciel::tuple<Types...>>& {
    using type = tuple_element_t<I, ciel::tuple<Types...>>;
    return static_cast<type&>(static_cast<ciel::details::tuple_leaf<I, type>&>(t.base_).get());
}

template<size_t I, class... Types>
[[nodiscard]] constexpr auto get(ciel::tuple<Types...>&& t) noexcept
    -> tuple_element_t<I, ciel::tuple<Types...>>&& {
    using type = tuple_element_t<I, ciel::tuple<Types...>>;
    return static_cast<type&&>(static_cast<ciel::details::tuple_leaf<I, type>&&>(t.base_).get());
}

template<size_t I, class... Types>
[[nodiscard]] constexpr auto get(const ciel::tuple<Types...>& t) noexcept
    -> tuple_element_t<I, ciel::tuple<Types...>> const& {
    using type = tuple_element_t<I, ciel::tuple<Types...>>;
    return static_cast<const type&>(static_cast<const ciel::details::tuple_leaf<I, type>&>(t.base_).get());
}

template<size_t I, class... Types>
[[nodiscard]] constexpr auto get(const ciel::tuple<Types...>&& t) noexcept
    -> tuple_element_t<I, ciel::tuple<Types...>> const&& {
    using type = tuple_element_t<I, ciel::tuple<Types...>>;
    return static_cast<const type&&>(static_cast<const ciel::details::tuple_leaf<I, type>&&>(t.base_).get());
}

template<class T, class... Types>
[[nodiscard]] constexpr auto get(ciel::tuple<Types...>& t) noexcept -> T& {
    constexpr size_t index = ciel::find_index_in_Args_v<0, T, Types...>;
    return static_cast<T&>(static_cast<ciel::details::tuple_leaf<index, T>&>(t.base_).get());
}

template<class T, class... Types>
[[nodiscard]] constexpr auto get(ciel::tuple<Types...>&& t) noexcept -> T&& {
    constexpr size_t index = ciel::find_index_in_Args_v<0, T, Types...>;
    return static_cast<T&&>(static_cast<ciel::details::tuple_leaf<index, T>&&>(t.base_).get());
}

template<class T, class... Types>
[[nodiscard]] constexpr auto get(const ciel::tuple<Types...>& t) noexcept -> const T& {
    constexpr size_t index = ciel::find_index_in_Args_v<0, T, Types...>;
    return static_cast<const T&>(static_cast<const ciel::details::tuple_leaf<index, T>&>(t.base_).get());
}

template<class T, class... Types>
[[nodiscard]] constexpr auto get(const ciel::tuple<Types...>&& t) noexcept -> const T&& {
    constexpr size_t index = ciel::find_index_in_Args_v<0, T, Types...>;
    return static_cast<const T&&>(static_cast<const ciel::details::tuple_leaf<index, T>&&>(t.base_).get());
}

}   // namespace std

NAMESPACE_CIEL_BEGIN

// TODO: constructors with allocator

namespace details {

template<size_t, class T>
struct tuple_leaf {
private:
    T t_;

public:
    constexpr explicit tuple_leaf() = default;
    constexpr tuple_leaf(const tuple_leaf&) = default;
    constexpr tuple_leaf(tuple_leaf&&) noexcept = default;
    constexpr ~tuple_leaf() = default;
    constexpr auto operator=(const tuple_leaf&) -> tuple_leaf& = default;
    constexpr auto operator=(tuple_leaf&&) noexcept -> tuple_leaf& = default;

    template<class U>
    constexpr explicit tuple_leaf(U&& u) : t_(std::forward<U>(u)) {}

    constexpr auto swap(tuple_leaf& other) noexcept -> void {
        using std::swap;
        swap(t_, other.t_);
    }

    [[nodiscard]] constexpr auto get() noexcept -> T& {
        return t_;
    }

    [[nodiscard]] constexpr auto get() const noexcept -> const T& {
        return t_;
    }

};  // struct tuple_leaf

template<size_t I, class T>
    requires (is_empty_v<T> && !is_final_v < T >)    // empty base optimazation
struct tuple_leaf<I, T> : T {
    constexpr explicit tuple_leaf() = default;
    constexpr tuple_leaf(const tuple_leaf&) = default;
    constexpr tuple_leaf(tuple_leaf&&) noexcept = default;
    constexpr ~tuple_leaf() = default;
    constexpr auto operator=(const tuple_leaf&) -> tuple_leaf& = default;
    constexpr auto operator=(tuple_leaf&&) noexcept -> tuple_leaf& = default;

    template<class U>
    constexpr explicit tuple_leaf(U&& u) : T(std::forward<U>(u)) {}

    constexpr auto swap(tuple_leaf& other) noexcept -> void {
        using std::swap;
        swap(*this, other);
    }

    [[nodiscard]] constexpr auto get() noexcept -> T& {
        return *this;
    }

    [[nodiscard]] constexpr auto get() const noexcept -> const T& {
        return *this;
    }

};  // struct tuple_leaf<I, T

template<class Index, class... Types>
struct tuple_impl;

template<size_t... I, class... Types>
struct tuple_impl<index_sequence<I...>, Types...> : tuple_leaf<I, Types> ... {
    constexpr explicit tuple_impl() = default;
    constexpr tuple_impl(const tuple_impl&) = default;
    constexpr tuple_impl(tuple_impl&&) noexcept = default;
    constexpr ~tuple_impl() = default;
    constexpr auto operator=(const tuple_impl&) -> tuple_impl& = default;
    constexpr auto operator=(tuple_impl&&) noexcept -> tuple_impl& = default;

    template<class... UTypes>
    constexpr explicit tuple_impl(UTypes&& ... args) : tuple_leaf<I, Types>(std::forward<UTypes>(args))... {}

    // TODO: Tuple should satisfy tuple-like concept, or it will be confused with single argument constructor
    template<class Tuple>
        requires requires(Tuple t) { std::get<0>(t); }
    constexpr explicit tuple_impl(Tuple&& t)
        : tuple_leaf<I, Types>(std::forward<copy_cvref_t<Tuple, std::tuple_element_t<I, remove_cvref_t<Tuple>>>>(
            std::get<I>(t)))... {}

    constexpr auto swap(tuple_impl& other) noexcept -> void {
        (static_cast<tuple_leaf<I, Types>&>(*this).swap(static_cast<tuple_leaf<I, Types>&>(other)), ...);
    }

};  // struct tuple_impl<index_sequence<I...>, Types...>

}   // namespace details

template<class...>
struct take_first_arg;

template<class Front, class... Rest>
struct take_first_arg<Front, Rest...> {
    using type = Front;
};

template<class... Args>
using take_first_arg_t = typename take_first_arg<Args...>::type;

template<class>
struct is_tuple : false_type {};

template<class... Types>
struct is_tuple<tuple<Types...>> : true_type {};

template<class T>
inline constexpr bool is_tuple_v = is_tuple<T>::value;

template<class Destination, class Source, size_t... I>
constexpr auto memberwise_copy_assign(Destination& d,
                                      const Source& s,
                                      index_sequence<I...> /*unused*/) -> void {
    ((std::get<I>(d) = std::get<I>(s)), ...);
}

template<class Destination, class Source, class... Types, size_t... I>
constexpr auto memberwise_forward_assign(Destination& d,
                                         Source&& s,
                                         tuple_types<Types...> /*unused*/,
                                         index_sequence<I...> /*unused*/) -> void {
    ((std::get<I>(d) = std::forward<Types>(std::get<I>(s))), ...);
}

template<class... Types>
struct tuple {
private:
    using base_type = details::tuple_impl<index_sequence_for<Types...>, Types...>;

    base_type base_;

    template<size_t I, class... Ts>
    friend constexpr auto std::get(tuple<Ts...>&) noexcept -> std::tuple_element_t<I, tuple<Ts...>>&;
    template<size_t I, class... Ts>
    friend constexpr auto std::get(tuple<Ts...>&&) noexcept -> std::tuple_element_t<I, tuple<Ts...>>&&;
    template<size_t I, class... Ts>
    friend constexpr auto std::get(const tuple<Ts...>&) noexcept -> std::tuple_element_t<I, tuple<Ts...>> const&;
    template<size_t I, class... Ts>
    friend constexpr auto std::get(const tuple<Ts...>&&) noexcept -> std::tuple_element_t<I, tuple<Ts...>> const&&;
    template<class T, class... Ts>
    friend constexpr auto std::get(tuple<Ts...>&) noexcept -> T&;
    template<class T, class... Ts>
    friend constexpr auto std::get(tuple<Ts...>&&) noexcept -> T&&;
    template<class T, class... Ts>
    friend constexpr auto std::get(const tuple<Ts...>&) noexcept -> const T&;
    template<class T, class... Ts>
    friend constexpr auto std::get(const tuple<Ts...>&&) noexcept -> const T&&;

public:
    constexpr explicit(!(is_implicitly_default_constructible_v<Types> && ...)) tuple()
        requires (is_default_constructible_v<Types> && ...) = default;

    constexpr explicit(!(is_convertible_v<const Types&, Types> && ...)) tuple(const Types& ... args)
        requires (is_copy_constructible_v<Types> && ...) : base_(args...) {}

    template<class... UTypes>
        requires (sizeof...(Types) == sizeof...(UTypes))
            && (is_constructible_v<Types, UTypes>
            && ...)
            && ((sizeof...(Types) == 1 && !is_tuple_v<remove_cvref_t<take_first_arg_t<UTypes...>>>)
            || ((sizeof...(Types) == 2 || sizeof...(Types) == 3)
            && (!is_same_v<remove_cvref_t<take_first_arg_t<UTypes...>>, allocator_arg_t>
            || is_same_v<take_first_arg_t<Types...>, allocator_arg_t>)))
    constexpr explicit(!(is_convertible_v<UTypes, Types> && ...)) tuple(UTypes&& ... args)
        : base_(std::forward<UTypes>(args)...) {}

    template<class OtherTuple, class = remove_cvref_t<OtherTuple>>
    struct enable_ctor_from_UTypes_tuple : false_type {};

    template<class OtherTuple, class... UTypes>
        requires (sizeof...(Types) == sizeof...(UTypes))
            && (is_constructible_v<Types, copy_cvref_t<OtherTuple, UTypes>> && ...)
            && (sizeof...(Types) != 1
            || ((!is_convertible_v<OtherTuple, Types> && ...)
            && (!is_constructible_v<Types, OtherTuple> && ...)
            && (!is_same_v<Types, UTypes> && ...)))
    struct enable_ctor_from_UTypes_tuple<OtherTuple, tuple<UTypes...>> : true_type {};

    template<class... UTypes>
    constexpr explicit(!(is_convertible_v<const UTypes&, Types> && ...)) tuple(const tuple<UTypes...>& other)
        requires(enable_ctor_from_UTypes_tuple<decltype(other)>::value)
        : base_(std::forward<decltype(other)>(other)) {}

    template<class... UTypes>
    constexpr explicit(!(is_convertible_v<UTypes&&, Types> && ...)) tuple(tuple<UTypes...>&& other)
        requires(enable_ctor_from_UTypes_tuple<decltype(other)>::value)
        : base_(std::forward<decltype(other)>(other)) {}

    template<class OtherPair, class = remove_cvref_t<OtherPair>>
    struct enable_ctor_from_pair : false_type {};

    template<class OtherPair, class First, class Second>
        requires (sizeof...(Types) == 2)
            && (is_constructible_v<std::tuple_element_t<0, tuple>, copy_cvref_t<OtherPair, First>>
            && is_constructible_v<std::tuple_element_t<1, tuple>, copy_cvref_t<OtherPair, Second>>)
    struct enable_ctor_from_pair<OtherPair, pair<First, Second>> : true_type {};

    template<class U1, class U2>
        requires enable_ctor_from_pair<const pair<U1, U2>&>::value
            && (!is_convertible_v<const U1&, std::tuple_element_t<0, tuple>>
            || !is_convertible_v<const U2&, std::tuple_element_t<1, tuple>>)
    constexpr explicit tuple(const pair<U1, U2>& p) : base_(std::forward<decltype(p)>(p)) {}

    template<class U1, class U2>
        requires enable_ctor_from_pair<const pair<U1, U2>&>::value
    constexpr tuple(const pair<U1, U2>& p) : base_(std::forward<decltype(p)>(p)) {}

    template<class U1, class U2>
        requires enable_ctor_from_pair<pair<U1, U2>&&>::value
            && (!is_convertible_v<U1&&, std::tuple_element_t<0, tuple>>
            || !is_convertible_v<U2&&, std::tuple_element_t<1, tuple>>)
    constexpr explicit tuple(pair<U1, U2>&& p) : base_(std::forward<decltype(p)>(p)) {}

    template<class U1, class U2>
        requires enable_ctor_from_pair<pair<U1, U2>&&>::value
    constexpr tuple(pair<U1, U2>&& p) : base_(std::forward<decltype(p)>(p)) {}

    constexpr tuple(const tuple&) = default;

    constexpr tuple(tuple&&) noexcept = default;

    constexpr ~tuple() = default;

    constexpr auto operator=(const tuple& other) -> tuple&
        requires (is_copy_assignable_v<Types> && ...) {
        if (this == addressof(other)) {
            return *this;
        }
        memberwise_copy_assign(*this, other, index_sequence_for<Types...>());
        return *this;
    }

    constexpr auto operator=(tuple&& other) noexcept((is_nothrow_move_assignable_v<Types> && ...)) -> tuple&
        requires (is_move_assignable_v<Types> && ...) {
        if (this == addressof(other)) {
            return *this;
        }
        memberwise_forward_assign(*this, std::move(other), tuple_types<Types...>(), index_sequence_for<Types...>());
        return *this;
    }

    template<class... UTypes>
        requires (sizeof...(Types) == sizeof...(UTypes)) && (is_assignable_v<Types&, const UTypes&> && ...)
    constexpr auto operator=(const tuple<UTypes...>& other) -> tuple& {
        memberwise_copy_assign(*this, other, index_sequence_for<Types...>());
        return *this;
    }

    template<class... UTypes>
        requires (sizeof...(Types) == sizeof...(UTypes))
            && (is_assignable_v<Types&, UTypes> && ...)
    constexpr auto operator=(tuple<UTypes...>&& other) -> tuple& {
        memberwise_forward_assign(*this, std::move(other), tuple_types<UTypes...>(), index_sequence_for<Types...>());
        return *this;
    }

    template<class E1, class E2>
        requires (sizeof...(Types) == 2)
            && is_assignable_v<std::tuple_element_t<0, tuple>&, const E1&>
            && is_assignable_v<std::tuple_element_t<1, tuple>&, const E2&>
    constexpr auto operator=(const pair<E1, E2>& p) -> tuple& {
        memberwise_copy_assign(*this, p, index_sequence_for<Types...>());
        return *this;
    }

    template<class E1, class E2>
        requires (sizeof...(Types) == 2)
            && is_assignable_v<std::tuple_element_t<0, tuple>&, E1>
            && is_assignable_v<std::tuple_element_t<1, tuple>&, E2>
    constexpr auto operator=(pair<E1, E2>&& p) -> tuple& {
        memberwise_forward_assign(*this, std::move(p), tuple_types<E1, E2>(), index_sequence_for<Types...>());
        return *this;
    }

    constexpr auto swap(tuple& other) noexcept((is_nothrow_swappable_v<Types> && ...)) -> void
        requires (is_swappable_v<Types> && ...) {
        base_.swap(other.base_);
    }

};    // struct tuple

template<>
struct tuple<> {
    constexpr auto swap(tuple& /*unused*/) noexcept -> void {}

};  // struct tuple<>

template<class... Types>
[[nodiscard]] constexpr auto make_tuple(Types&& ... args) -> tuple<typename unwrap_ref<decay_t<Types>>::type...> {
    return tuple<typename unwrap_ref<decay_t<Types>>::type...>(std::forward<Types>(args)...);
}

template<class... Args>
[[nodiscard]] constexpr auto tie(Args& ... args) noexcept -> tuple<Args & ...> {
    return {args...};
}

template<class... Types>
[[nodiscard]] constexpr auto forward_as_tuple(Types&& ... args) noexcept -> tuple<Types && ...> {
    return tuple<Types&& ...>(std::forward<Types>(args)...);
}

// TODO: tuple_cat

namespace details {

template<class... TTypes, class... UTypes, size_t... I>
[[nodiscard]] constexpr auto tuple_equal(const tuple<TTypes...>& lhs,
                                         const tuple<UTypes...>& rhs,
                                         index_sequence<I...> /*unused*/) -> bool {
    return ((std::get<I>(lhs) == std::get<I>(rhs)) && ...);
}

template<class... TTypes, class... UTypes, size_t... I>
[[nodiscard]] constexpr auto tuple_compare_three_way(const tuple<TTypes...>& lhs,
                                                     const tuple<UTypes...>& rhs,
                                                     index_sequence<I...> /*unused*/) {
    std::common_comparison_category_t<synth_three_way_result<TTypes, UTypes>...> res = std::strong_ordering::equal;
    (void)((res = ciel::synth_three_way(std::get<I>(lhs), std::get<I>(rhs)), res != 0) || ...);
    return res;
}

}   // namespace details

template<class... TTypes, class... UTypes>
    requires (sizeof...(TTypes) == sizeof...(UTypes))
[[nodiscard]] constexpr auto operator==(const tuple<TTypes...>& lhs, const tuple<UTypes...>& rhs) -> bool {
    return details::tuple_equal(lhs, rhs, index_sequence_for<TTypes...>());
}

template<class... TTypes, class... UTypes>
    requires (sizeof...(TTypes) == sizeof...(UTypes))
[[nodiscard]] constexpr auto operator<=>(const tuple<TTypes...>& lhs,
                                         const tuple<UTypes...>& rhs) -> std::common_comparison_category_t<
    synth_three_way_result<TTypes, UTypes>...> {
    return details::tuple_compare_three_way(lhs, rhs, index_sequence_for<TTypes...>());
}

template<class... Types, class Alloc>
struct uses_allocator<tuple<Types...>, Alloc> : true_type {};

namespace details {

struct ignore_t {
    template<class T>
    constexpr auto operator=(T&& /*unused*/) const noexcept -> void {}
};

}   // namespace details

inline constexpr details::ignore_t ignore;

template<class... UTypes>
tuple(UTypes...) -> tuple<UTypes...>;

template<class T1, class T2>
tuple(pair<T1, T2>) -> tuple<T1, T2>;

NAMESPACE_CIEL_END

namespace std {

template<class... Types>
    requires (ciel::is_swappable_v<Types> && ...)
constexpr auto swap(ciel::tuple<Types...>& lhs, ciel::tuple<Types...>& rhs) noexcept(noexcept(lhs.swap(rhs))) -> void {
    lhs.swap(rhs);
}

}   // namespace std

#endif // CIELUTILS_INCLUDE_CIEL_TUPLE_HPP_