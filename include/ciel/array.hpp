#ifndef CIELUTILS_INCLUDE_CIEL_ARRAY_HPP_
#define CIELUTILS_INCLUDE_CIEL_ARRAY_HPP_

#include <ciel/algorithm_impl/equal.hpp>
#include <ciel/algorithm_impl/fill_n.hpp>
#include <ciel/algorithm_impl/swap_ranges.hpp>
#include <ciel/config.hpp>
#include <ciel/iterator_impl/reverse_iterator.hpp>
#include <ciel/type_traits_impl/enable_if.hpp>
#include <ciel/type_traits_impl/is_swappable.hpp>
#include <ciel/utility_impl/integer_sequence.hpp>
#include <cstddef>
#include <stdexcept>

NAMESPACE_CIEL_BEGIN

template<class T, size_t N>
struct array {

    static_assert(N != 0, "ciel::array with size 0 is not supported");

    using value_type             = T;
    using size_type              = size_t;
    using difference_type        = ptrdiff_t;
    using reference              = value_type&;
    using const_reference        = const value_type&;
    using pointer                = value_type*;
    using const_pointer          = const value_type*;

    using iterator               = value_type*;
    using const_iterator         = const value_type*;

    using reverse_iterator       = ciel::reverse_iterator<iterator>;
    using const_reverse_iterator = ciel::reverse_iterator<const_iterator>;

    T data_[N];

    [[nodiscard]] constexpr auto at(const size_type pos) -> reference {
        if (pos >= N) {
            THROW(std::out_of_range("pos is not within the range of ciel::array"));
        }
        return data_[pos];
    }

    [[nodiscard]] constexpr auto at(const size_type pos) const -> const_reference {
        if (pos >= N) {
            THROW(std::out_of_range("pos is not within the range of ciel::array"));
        }
        return data_[pos];
    }

    [[nodiscard]] constexpr auto operator[](const size_type pos) -> reference {
        return data_[pos];
    }

    [[nodiscard]] constexpr auto operator[](const size_type pos) const -> const_reference {
        return data_[pos];
    }

    [[nodiscard]] constexpr auto front() -> reference {
        return data_[0];
    }

    [[nodiscard]] constexpr auto front() const -> const_reference {
        return data_[0];
    }

    [[nodiscard]] constexpr auto back() -> reference {
        return data_[N - 1];
    }

    [[nodiscard]] constexpr auto back() const -> const_reference {
        return data_[N - 1];
    }

    [[nodiscard]] constexpr auto data() noexcept -> T* {
        return data_;
    }

    [[nodiscard]] constexpr auto data() const noexcept -> const T* {
        return data_;
    }

    [[nodiscard]] constexpr auto begin() noexcept -> iterator {
        return iterator(data_);
    }

    [[nodiscard]] constexpr auto begin() const noexcept -> const_iterator {
        return const_iterator(data_);
    }

    [[nodiscard]] constexpr auto cbegin() const noexcept -> const_iterator {
        return begin();
    }

    [[nodiscard]] constexpr auto end() noexcept -> iterator {
        return iterator(data_ + N);
    }

    [[nodiscard]] constexpr auto end() const noexcept -> const_iterator {
        return const_iterator(data_ + N);
    }

    [[nodiscard]] constexpr auto cend() const noexcept -> const_iterator {
        return end();
    }

    [[nodiscard]] constexpr auto rbegin() noexcept -> reverse_iterator {
        return reverse_iterator(end());
    }

    [[nodiscard]] constexpr auto rbegin() const noexcept -> const_reverse_iterator {
        return const_reverse_iterator(end());
    }

    [[nodiscard]] constexpr auto crbegin() const noexcept -> const_reverse_iterator {
        return rbegin();
    }

    [[nodiscard]] constexpr auto rend() noexcept -> reverse_iterator {
        return reverse_iterator(begin());
    }

    [[nodiscard]] constexpr auto rend() const noexcept -> const_reverse_iterator {
        return const_reverse_iterator(begin());
    }

    [[nodiscard]] constexpr auto crend() const noexcept -> const_reverse_iterator {
        return rend();
    }

    [[nodiscard]] static constexpr auto empty() noexcept -> bool {
        return false;
    }

    [[nodiscard]] static constexpr auto size() noexcept -> size_type {
        return N;
    }

    [[nodiscard]] static constexpr auto max_size() noexcept -> size_type {
        return N;
    }

    constexpr auto fill(const T& value) -> void {
        ciel::fill_n(data_, N, value);
    }

    constexpr auto swap(array& other) noexcept(is_nothrow_swappable_v<T>) -> void {
        ciel::swap_ranges(data_, data_ + N, other.data_);
    }

};  // struct array

template<class T, size_t N>
[[nodiscard]] constexpr auto operator==(const array<T, N>& lhs, const array<T, N>& rhs) -> bool {
    return equal(lhs.begin(), lhs.end(), rhs.begin());
}

namespace details {

template<class T, size_t N, size_t... I>
[[nodiscard]] constexpr auto to_array_impl(T (& a)[N], index_sequence<I...> /*unused*/)
    -> array<remove_cv_t<T>, N> {
    return {{a[I]...}};
}

template<class T, size_t N, size_t... I>
[[nodiscard]] constexpr auto to_array_impl(T (&& a)[N], index_sequence<I...> /*unused*/)
    -> array<remove_cv_t<T>, N> {
    return {{std::move(a[I])...}};
}

}   // namespace details

template<class T, size_t N>
[[nodiscard]] constexpr auto to_array(T (& a)[N]) -> array<remove_cv_t<T>, N> {
    static_assert(!is_array_v<T>, "T is array");
    static_assert(is_constructible_v<T, T&>, "T is not copy constructible");

    return details::to_array_impl(a, make_index_sequence<N>{});
}

template<class T, size_t N>
[[nodiscard]] constexpr auto to_array(T (&& a)[N]) -> array<remove_cv_t<T>, N> {
    static_assert(!is_array_v<T>, "T is array");
    static_assert(is_move_constructible_v<T>, "T is not move constructible");

    return details::to_array_impl(std::move(a), make_index_sequence<N>{});
}

template<class T, class... U, class = enable_if_t<(is_same_v<T, U> && ...)>>
array(T, U...) -> array<T, 1 + sizeof...(U)>;

NAMESPACE_CIEL_END

namespace std {

template<size_t I, class T, size_t N>
[[nodiscard]] constexpr auto get(ciel::array<T, N>& a) noexcept -> T& {
    static_assert(I < N, "get(ciel::array) out of bound");

    return a[I];
}

template<size_t I, class T, size_t N>
[[nodiscard]] constexpr auto get(ciel::array<T, N>&& a) noexcept -> T&& {
    static_assert(I < N, "get(ciel::array) out of bound");

    return std::move(a[I]);
}

template<size_t I, class T, size_t N>
[[nodiscard]] constexpr auto get(const ciel::array<T, N>& a) noexcept -> const T& {
    static_assert(I < N, "get(ciel::array) out of bound");

    return a[I];
}

template<size_t I, class T, size_t N>
[[nodiscard]] constexpr auto get(const ciel::array<T, N>&& a) noexcept -> const T&& {
    static_assert(I < N, "get(ciel::array) out of bound");

    return std::move(a[I]);
}

template<class T, size_t N>
constexpr auto swap(ciel::array<T, N>& lhs, ciel::array<T, N>& rhs) noexcept(noexcept(lhs.swap(rhs))) -> void {
    lhs.swap(rhs);
}

template<class T, size_t N>
struct tuple_size<ciel::array<T, N>> : integral_constant<size_t, N> {};

template<size_t I, class T, size_t N>
struct tuple_element<I, ciel::array<T, N>> {
    using type = T;
};

}   // namespace std

#endif // CIELUTILS_INCLUDE_CIEL_ARRAY_HPP_