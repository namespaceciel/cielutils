#ifndef CIELUTILS_INCLUDE_CIEL_SPAN_HPP_
#define CIELUTILS_INCLUDE_CIEL_SPAN_HPP_

#include <ciel/array.hpp>
#include <ciel/config.hpp>
#include <ciel/iterator_impl/contiguous_iterator.hpp>
#include <ciel/iterator_impl/wrap_iter.hpp>
#include <ciel/limits.hpp>

NAMESPACE_CIEL_BEGIN

inline constexpr size_t dynamic_extent = numeric_limits<size_t>::max();

template<class From, class To>
concept span_array_convertible = is_convertible_v<From(*)[], To(*)[]>;

template<size_t Count, size_t Extent, size_t Offset>
[[nodiscard]] constexpr auto calculate_E() -> size_t {
    if constexpr (Count != dynamic_extent) {
        return Count;
    }
    if constexpr (Extent != dynamic_extent) {
        return Extent - Offset;
    }
    return dynamic_extent;
}

template<class T, size_t Extent = dynamic_extent>
class span {
public:
    using element_type     = T;
    using value_type       = remove_cv_t<T>;
    using size_type	       = size_t;
    using difference_type  = ptrdiff_t;
    using pointer          = T*;
    using const_pointer    = const T*;
    using reference        = T&;
    using const_reference  = const T&;
    using iterator         = wrap_iter<pointer>;
    using reverse_iterator = ciel::reverse_iterator<iterator>;

    static constexpr size_type extent = Extent;

private:
    pointer data_;

public:
    // We don't check if extent is equal to the real size of containers users give

    constexpr span() noexcept requires (extent == 0) : data_(nullptr) {}

    template<contiguous_iterator It>
        requires span_array_convertible<remove_reference_t<iter_reference_t<It>>, element_type>
    constexpr explicit span(It first, size_type /*unused*/) : data_(ciel::to_address(first)) {}

    template<contiguous_iterator It, sized_sentinel_for<It> End>
        requires span_array_convertible<remove_reference_t<iter_reference_t<It>>, element_type> &&
            (!is_convertible_v<End, size_t>)
    constexpr explicit span(It first, End /*unused*/) : data_(ciel::to_address(first)) {}

    constexpr span(element_type (&arr)[extent]) noexcept : data_(arr) {}

    template<class U>
    constexpr span(array<U, extent>& arr) noexcept requires (span_array_convertible<U, element_type>)
        : data_(arr.data()) {}

    template<class U>
    constexpr span(const array<U, extent>& arr) noexcept requires (span_array_convertible<U, element_type>)
        : data_(arr.data()) {}

    template<class U, size_t N>
        requires (N == dynamic_extent || extent == N) && span_array_convertible<U, element_type>
    constexpr explicit(N == dynamic_extent) span(const span<U, N>& source) noexcept : data_(source.data()) {}

    [[nodiscard]] constexpr auto begin() const noexcept -> iterator {
        return iterator(data());
    }

    [[nodiscard]] constexpr auto end() const noexcept -> iterator {
        return iterator(data() + size());
    }

    [[nodiscard]] constexpr auto rbegin() const noexcept -> reverse_iterator {
        return reverse_iterator(end());
    }

    [[nodiscard]] constexpr auto rend() const noexcept -> reverse_iterator {
        return reverse_iterator(begin());
    }

    [[nodiscard]] constexpr auto front() const -> reference {
        return (*this)[0];
    }

    [[nodiscard]] constexpr auto back() const -> reference {
        return (*this)[size() - 1];
    }

    [[nodiscard]] constexpr auto operator[](const size_type idx) const -> reference {
        return data()[idx];
    }

    [[nodiscard]] constexpr auto data() const noexcept -> pointer {
        return data_;
    }

    [[nodiscard]] constexpr auto size() const noexcept -> size_type {
        return extent;
    }

    [[nodiscard]] constexpr auto size_bytes() const noexcept -> size_type {
        return size() * sizeof(element_type);
    }

    [[nodiscard]] constexpr auto empty() const noexcept -> bool {
        return extent == 0;
    }

    template<size_t Count>
        requires (Count <= extent)
    [[nodiscard]] constexpr auto first() const -> span<element_type, Count> {
        return {data(), Count};
    }

    [[nodiscard]] constexpr auto first(const size_t Count) const -> span<element_type, dynamic_extent> {
        return {data(), Count};
    }

    template<size_t Count>
    [[nodiscard]] constexpr auto last() const -> span<element_type, Count> {
        return {data() + size() - Count, Count};
    }

    [[nodiscard]] constexpr auto last(const size_t Count) const -> span<element_type, dynamic_extent> {
        return {data() + size() - Count, Count};
    }

    template<size_t Offset, size_t Count = dynamic_extent, size_t E = calculate_E<Count, extent, Offset>()>
        requires (Offset <= extent) && (Count == dynamic_extent || Count <= extent - Offset)
    [[nodiscard]] constexpr auto subspan() const -> span<element_type, E> {
        return {data() + Offset, Count == dynamic_extent ? size() - Offset : Count};
    }

    [[nodiscard]] constexpr auto subspan(const size_t Offset, const size_t Count = dynamic_extent) const
        -> span<element_type, dynamic_extent> {
        return {data() + Offset, Count};
    }

};  // class span

template<class T>
class span<T, dynamic_extent> {
public:
    using element_type     = T;
    using value_type       = remove_cv_t<T>;
    using size_type    	   = size_t;
    using difference_type  = ptrdiff_t;
    using pointer          = T*;
    using const_pointer    = const T*;
    using reference        = T&;
    using const_reference  = const T&;
    using iterator         = wrap_iter<pointer>;
    using reverse_iterator = ciel::reverse_iterator<iterator>;

    static constexpr size_type extent = dynamic_extent;

private:
    pointer data_;
    size_type size_;

public:
    constexpr span() noexcept : data_(nullptr), size_(0) {}

    template<contiguous_iterator It>
        requires span_array_convertible<remove_reference_t<iter_reference_t<It>>, element_type>
    constexpr span(It first, const size_type count) : data_(ciel::to_address(first)), size_(count) {}

    template<contiguous_iterator It, sized_sentinel_for<It> End>
        requires span_array_convertible<remove_reference_t<iter_reference_t<It>>, element_type> &&
            (!is_convertible_v<End, size_t>)
    constexpr span(It first, End last) : data_(ciel::to_address(first)), size_(last - first) {}

    template<size_t N>
    constexpr span(element_type (&arr)[N]) noexcept : data_(arr), size_(N) {}

    template<class U, size_t N>
    constexpr span(array<U, N>& arr) noexcept requires (span_array_convertible<U, element_type>)
        : data_(arr.data()), size_(N) {}

    template<class U, size_t N>
    constexpr span(const array<U, N>& arr) noexcept requires (span_array_convertible<U, element_type>)
        : data_(arr.data()), size_(N) {}

    template<class U, size_t N>
        requires span_array_convertible<U, element_type>
    constexpr span(const span<U, N>& source) noexcept : data_(source.data()), size_(source.size()) {}

    [[nodiscard]] constexpr auto begin() const noexcept -> iterator {
        return iterator(data());
    }

    [[nodiscard]] constexpr auto end() const noexcept -> iterator {
        return iterator(data() + size());
    }

    [[nodiscard]] constexpr auto rbegin() const noexcept -> reverse_iterator {
        return reverse_iterator(end());
    }

    [[nodiscard]] constexpr auto rend() const noexcept -> reverse_iterator {
        return reverse_iterator(begin());
    }

    [[nodiscard]] constexpr auto front() const -> reference {
        return (*this)[0];
    }

    [[nodiscard]] constexpr auto back() const -> reference {
        return (*this)[size() - 1];
    }

    [[nodiscard]] constexpr auto operator[](const size_type idx) const -> reference {
        return data()[idx];
    }

    [[nodiscard]] constexpr auto data() const noexcept -> pointer {
        return data_;
    }

    [[nodiscard]] constexpr auto size() const noexcept -> size_type {
        return size_;
    }

    [[nodiscard]] constexpr auto size_bytes() const noexcept -> size_type {
        return size() * sizeof(element_type);
    }

    [[nodiscard]] constexpr auto empty() const noexcept -> bool {
        return size() == 0;
    }

    template<size_t Count>
    [[nodiscard]] constexpr auto first() const -> span<element_type, Count> {
        return {data(), Count};
    }

    [[nodiscard]] constexpr auto first(const size_t Count) const -> span<element_type, dynamic_extent> {
        return {data(), Count};
    }

    template<size_t Count>
    [[nodiscard]] constexpr auto last() const -> span<element_type, Count> {
        return {data() + size() - Count, Count};
    }

    [[nodiscard]] constexpr auto last(const size_t Count) const -> span<element_type, dynamic_extent> {
        return {data() + size() - Count, Count};
    }

    template<size_t Offset, size_t Count = dynamic_extent, size_t E = calculate_E<Count, extent, Offset>()>
        requires (Offset <= extent) && (Count == dynamic_extent || Count <= extent - Offset)
    [[nodiscard]] constexpr auto subspan() const -> span<element_type, E> {
        return {data() + Offset, Count == dynamic_extent ? size() - Offset : Count};
    }

    [[nodiscard]] constexpr auto subspan(const size_t Offset, const size_t Count = dynamic_extent) const
        -> span<element_type, dynamic_extent> {
        return {data() + Offset, Count};
    }

};  // class span<T, dynamic_extent>

template<class T, size_t N>
[[nodiscard]] constexpr auto calculate_S() -> size_t {
    if constexpr (N == dynamic_extent) {
        return dynamic_extent;
    }
    return sizeof(T) * N;
}

template<class T, size_t N, size_t S = calculate_S<T, N>()>
[[nodiscard]] auto as_bytes(span<T, N> s) noexcept -> span<const std::byte, S> {
    return {reinterpret_cast<const std::byte*>(s.data()), s.size_bytes()};
}

template<class T, size_t N, size_t S = calculate_S<T, N>()>
    requires (!is_const_v<T>)
[[nodiscard]] auto as_writable_bytes(span<T, N> s) noexcept -> span<std::byte, S> {
    return {reinterpret_cast<std::byte*>(s.data()), s.size_bytes()};
}

template<contiguous_iterator It, class EndOrSize>
span(It, EndOrSize) -> span<remove_reference_t<iter_reference_t<It>>>;

template<class T, size_t N>
span(T (&)[N]) -> span<T, N>;

template<class T, size_t N>
span(array<T, N>&) -> span<T, N>;

template<class T, size_t N>
span(const array<T, N>&) -> span<const T, N>;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_SPAN_HPP_