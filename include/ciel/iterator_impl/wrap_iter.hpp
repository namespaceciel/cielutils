#ifndef CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_WRAP_ITER_HPP_
#define CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_WRAP_ITER_HPP_

#include <ciel/config.hpp>
#include <ciel/iterator_impl/iterator_traits.hpp>
#include <ciel/memory_impl/to_address.hpp>

NAMESPACE_CIEL_BEGIN

template<class Iter>
class wrap_iter {
public:
    using iterator_type     = Iter;
    using value_type        = typename iterator_traits<iterator_type>::value_type;
    using difference_type   = typename iterator_traits<iterator_type>::difference_type;
    using pointer           = typename iterator_traits<iterator_type>::pointer;
    using reference         = typename iterator_traits<iterator_type>::reference;
    using iterator_category = typename iterator_traits<iterator_type>::iterator_category;
    using iterator_concept  = contiguous_iterator_tag;

private:
    iterator_type it;

public:
    constexpr explicit wrap_iter(iterator_type other) noexcept: it(other) {}

    constexpr wrap_iter() noexcept : it() {}

    constexpr wrap_iter(const wrap_iter&) noexcept = default;

    template<class U>
    constexpr wrap_iter(const wrap_iter<U>& other) noexcept : it(const_cast<iterator_type>(other.base())) {}

    constexpr wrap_iter(wrap_iter&&) noexcept = default;

    constexpr ~wrap_iter() = default;

    constexpr auto operator=(const wrap_iter&) noexcept -> wrap_iter& = default;

    constexpr auto operator=(wrap_iter&&) noexcept -> wrap_iter& = default;

    [[nodiscard]] constexpr auto operator*() const noexcept -> reference {
        return *it;
    }

    [[nodiscard]] constexpr auto operator->() const noexcept -> pointer {
        return ciel::to_address(it);
    }

    constexpr auto operator++() noexcept -> wrap_iter& {
        ++it;
        return *this;
    }

    [[nodiscard]] constexpr auto operator++(int) noexcept -> wrap_iter {
        wrap_iter res(*this);
        ++it;
        return res;
    }

    constexpr auto operator--() noexcept -> wrap_iter& {
        --it;
        return *this;
    }

    [[nodiscard]] constexpr auto operator--(int) noexcept -> wrap_iter {
        wrap_iter res(*this);
        --it;
        return res;
    }

    [[nodiscard]] constexpr auto operator+(const difference_type n) const noexcept -> wrap_iter {
        wrap_iter res(*this);
        res += n;
        return res;
    }

    constexpr auto operator+=(const difference_type n) noexcept -> wrap_iter& {
        it += n;
        return *this;
    }

    [[nodiscard]] constexpr auto operator-(const difference_type n) const noexcept -> wrap_iter {
        return *this + (-n);
    }

    constexpr auto operator-=(const difference_type n) noexcept -> wrap_iter& {
        *this += (-n);
        return *this;
    }

    [[nodiscard]] constexpr auto operator[](const difference_type n) const noexcept -> reference {
        return it[n];
    }

    [[nodiscard]] constexpr auto base() const noexcept -> iterator_type {
        return it;
    }

    [[nodiscard]] explicit operator bool() const noexcept {
        return base() != nullptr;
    }

};    // class wrap_iter

template<class Iter1, class Iter2>
[[nodiscard]] constexpr auto operator==(const wrap_iter<Iter1>& lhs, const wrap_iter<Iter2>& rhs) -> bool {
    return lhs.base() == rhs.base();
}

template<class Iter1, class Iter2>
[[nodiscard]] constexpr auto operator!=(const wrap_iter<Iter1>& lhs, const wrap_iter<Iter2>& rhs) -> bool {
    return !(lhs == rhs);
}

template<class Iter1, class Iter2>
[[nodiscard]] constexpr auto operator<(const wrap_iter<Iter1>& lhs, const wrap_iter<Iter2>& rhs) -> bool {
    return lhs.base() < rhs.base();
}

template<class Iter1, class Iter2>
[[nodiscard]] constexpr auto operator>(const wrap_iter<Iter1>& lhs, const wrap_iter<Iter2>& rhs) -> bool {
    return lhs.base() > rhs.base();
}

template<class Iter1, class Iter2>
[[nodiscard]] constexpr auto operator<=(const wrap_iter<Iter1>& lhs, const wrap_iter<Iter2>& rhs) -> bool {
    return !(lhs > rhs);
}

template<class Iter1, class Iter2>
[[nodiscard]] constexpr auto operator>=(const wrap_iter<Iter1>& lhs, const wrap_iter<Iter2>& rhs) -> bool {
    return !(lhs < rhs);
}

template<class Iter1, class Iter2>
[[nodiscard]] constexpr auto operator-(const wrap_iter<Iter1>& lhs, const wrap_iter<Iter2>& rhs) -> decltype(lhs.base() - rhs.base()) {
    return lhs.base() - rhs.base();
}

template<class Iter>
[[nodiscard]] constexpr auto operator+(const typename wrap_iter<Iter>::difference_type n, wrap_iter<Iter> wi) noexcept -> wrap_iter<Iter> {
    wi += n;
    return wi;
}

template<class Iter>
struct pointer_traits<wrap_iter<Iter>> {
    using pointer = wrap_iter<Iter>;
    using element_type = typename pointer_traits<Iter>::element_type;
    using difference_type = typename pointer_traits<Iter>::difference_type;

    [[nodiscard]] constexpr static auto to_address(pointer p) noexcept -> element_type* {
        return ciel::to_address(p.base());
    }
};

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_WRAP_ITER_HPP_