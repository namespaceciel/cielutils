#ifndef CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_REVERSE_ITERATOR_HPP_
#define CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_REVERSE_ITERATOR_HPP_

#include <ciel/config.hpp>
#include <ciel/iterator_impl/prev.hpp>
#include <ciel/iterator_impl/random_access_iterator.hpp>
#include <ciel/type_traits_impl/is_copy_constructible.hpp>
#include <ciel/type_traits_impl/is_pointer.hpp>

NAMESPACE_CIEL_BEGIN

namespace details {

template<class Iter>
struct what_iterator_concept {
    using iterator_concept = bidirectional_iterator_tag;
};

template<class Iter>
    requires random_access_iterator<Iter>
struct what_iterator_concept<Iter> {
    using iterator_concept = random_access_iterator_tag;
};

template<class Iter>
struct what_iterator_category {
    using iterator_category = typename iterator_traits<Iter>::iterator_category;
};

template<class Iter>
    requires derived_from<typename iterator_traits<Iter>::iterator_category, random_access_iterator_tag>
struct what_iterator_category<Iter> {
    using iterator_category = random_access_iterator_tag;
};

}   // namespace details

template<class Iter>
    requires legacy_bidirectional_iterator<Iter> || bidirectional_iterator<Iter>
class reverse_iterator {
public:
    using iterator_type     = Iter;
    using iterator_concept  = typename details::what_iterator_concept<Iter>::iterator_concept;
    using iterator_catetory = typename details::what_iterator_category<Iter>::iterator_category;
    using value_type        = iter_value_t<Iter>;
    using difference_type   = iter_difference_t<Iter>;
    using pointer           = typename iterator_traits<Iter>::pointer;
    using reference         = iter_reference_t<Iter>;

private:
    iterator_type current_;

public:
    constexpr reverse_iterator() : current_() {}

    constexpr explicit reverse_iterator(iterator_type x) : current_(x) {}

    template<class U>
        requires (!is_same_v<Iter, U> && convertible_to<const U&, Iter>)
    constexpr reverse_iterator(const reverse_iterator<U>& other):current_(other.base()) {}

    template<class U>
        requires (!is_same_v<Iter, U> && convertible_to<const U&, Iter> && assignable_from<Iter&, const U&>)
    constexpr auto operator=(const reverse_iterator<U>& other) -> reverse_iterator& {
        current_ = other.base();
        return *this;
    }

    [[nodiscard]] constexpr auto base() const -> iterator_type {
        return current_;
    }

    [[nodiscard]] constexpr auto operator*() const -> reference {
        iterator_type tmp = current_;
        return *--tmp;
    }

    [[nodiscard]] constexpr auto operator->() const -> pointer
        requires is_pointer_v<Iter> {
        return current_ - 1;
    }

    [[nodiscard]] constexpr auto operator->() const -> pointer
        requires (!is_pointer_v<Iter> && requires(const Iter i) { i.operator->(); }) {
        return ciel::prev(current_).operator->();
    }

    constexpr auto operator++() -> reverse_iterator& {
        --current_;
        return *this;
    }

    [[nodiscard]] constexpr auto operator++(int) -> reverse_iterator {
        reverse_iterator res(*this);
        --current_;
        return res;
    }

    constexpr auto operator--() -> reverse_iterator& {
        ++current_;
        return *this;
    }

    [[nodiscard]] constexpr auto operator--(int) -> reverse_iterator {
        reverse_iterator res(*this);
        ++current_;
        return res;
    }

    [[nodiscard]] constexpr auto operator+(const difference_type n) const -> reverse_iterator {
        return reverse_iterator(current_ - n);
    }

    constexpr auto operator+=(const difference_type n) -> reverse_iterator& {
        current_ -= n;
        return *this;
    }

    [[nodiscard]] constexpr auto operator-(const difference_type n) const -> reverse_iterator {
        return reverse_iterator(current_ + n);
    }

    constexpr auto operator-=(const difference_type n) -> reverse_iterator& {
        current_ += n;
        return *this;
    }

    [[nodiscard]] constexpr auto operator[](const difference_type n) const -> reference {
        return *(*this + n);
    }

    friend constexpr auto iter_move(const reverse_iterator& i)
        noexcept(is_nothrow_copy_constructible_v<Iter> &&
            noexcept(ranges::iter_move(--declval<Iter&>()))) -> iter_rvalue_reference_t<Iter> {
        auto tmp = i.base();
        return ranges::iter_move(--tmp);
    }

};    // class reverse_iterator

template<class Iter>
[[nodiscard]] constexpr auto make_reverse_iterator(Iter i) -> reverse_iterator<Iter> {
    return reverse_iterator<Iter>(i);
}

template<class Iterator1, class Iterator2>
[[nodiscard]] constexpr auto operator==(const reverse_iterator<Iterator1>& lhs,
                                        const reverse_iterator<Iterator2>& rhs) -> bool
    requires requires{{ lhs.base() == rhs.base() } -> boolean_testable; } {
    return lhs.base() == rhs.base();
}

template<class Iterator1, class Iterator2>
[[nodiscard]] constexpr auto operator!=(const reverse_iterator<Iterator1>& lhs,
                                        const reverse_iterator<Iterator2>& rhs) -> bool
    requires requires{{ lhs.base() != rhs.base() } -> boolean_testable; } {
    return lhs.base() != rhs.base();
}

template<class Iterator1, class Iterator2>
[[nodiscard]] constexpr auto operator<(const reverse_iterator<Iterator1>& lhs,
                                       const reverse_iterator<Iterator2>& rhs) -> bool
    requires requires{{ lhs.base() > rhs.base() } -> boolean_testable; } {
    return lhs.base() > rhs.base();
}

template<class Iterator1, class Iterator2>
[[nodiscard]] constexpr auto operator<=(const reverse_iterator<Iterator1>& lhs,
                                        const reverse_iterator<Iterator2>& rhs) -> bool
    requires requires{{ lhs.base() >= rhs.base() } -> boolean_testable; } {
    return lhs.base() >= rhs.base();
}

template<class Iterator1, class Iterator2>
[[nodiscard]] constexpr auto operator>(const reverse_iterator<Iterator1>& lhs,
                                       const reverse_iterator<Iterator2>& rhs) -> bool
    requires requires{{ lhs.base() < rhs.base() } -> boolean_testable; } {
    return lhs.base() < rhs.base();
}

template<class Iterator1, class Iterator2>
[[nodiscard]] constexpr auto operator>=(const reverse_iterator<Iterator1>& lhs,
                                        const reverse_iterator<Iterator2>& rhs) -> bool
    requires requires{{ lhs.base() <= rhs.base() } -> boolean_testable; } {
    return lhs.base() <= rhs.base();
}

template<class Iterator1, std::three_way_comparable_with<Iterator1> Iterator2>
[[nodiscard]] constexpr auto operator<=>(const reverse_iterator<Iterator1>& lhs,
                                         const reverse_iterator<Iterator2>& rhs)
    -> std::compare_three_way_result_t<Iterator1, Iterator2> {
    return rhs.base() <=> lhs.base();
}

template<class Iter>
[[nodiscard]] constexpr auto operator+(const typename reverse_iterator<Iter>::difference_type n,
                                       const reverse_iterator<Iter>& it) -> reverse_iterator<Iter> {
    return reverse_iterator<Iter>(it.base() - n);
}

template<class Iterator1, class Iterator2>
[[nodiscard]] constexpr auto operator-(const reverse_iterator<Iterator1>& lhs,
                                       const reverse_iterator<Iterator2>& rhs) -> decltype(rhs.base() - lhs.base()) {
    return rhs.base() - lhs.base();
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_REVERSE_ITERATOR_HPP_