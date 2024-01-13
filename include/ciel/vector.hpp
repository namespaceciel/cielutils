#ifndef CIELUTILS_INCLUDE_CIEL_VECTOR_HPP_
#define CIELUTILS_INCLUDE_CIEL_VECTOR_HPP_

#include <ciel/algorithm_impl/equal.hpp>
#include <ciel/algorithm_impl/move.hpp>
#include <ciel/algorithm_impl/remove.hpp>
#include <ciel/algorithm_impl/remove_if.hpp>
#include <ciel/algorithm_impl/rotate.hpp>
#include <ciel/config.hpp>
#include <ciel/iterator_impl/distance.hpp>
#include <ciel/iterator_impl/reverse_iterator.hpp>
#include <ciel/iterator_impl/wrap_iter.hpp>
#include <ciel/memory_impl/allocator.hpp>
#include <ciel/memory_impl/allocator_traits.hpp>
#include <cstddef>
#include <stdexcept>

NAMESPACE_CIEL_BEGIN

// Differences between std::vector and this class:
// We don't provide specialization of vector for bool
// T is not allowed to throw in move constructor/assignment
// emplace, insert and resize has strong exception safety

template<class T, class Allocator = allocator<T>>
class vector {

    static_assert(is_same_v<typename Allocator::value_type, T>);
    static_assert(is_nothrow_move_constructible_v<T> && is_nothrow_move_assignable_v<T>,
                            "T is not allowed to throw in move constructor/assignment");

public:
    using value_type             = T;
    using allocator_type         = Allocator;
    using size_type              = size_t;
    using difference_type        = ptrdiff_t;
    using reference              = value_type&;
    using const_reference        = const value_type&;
    using pointer                = typename allocator_traits<allocator_type>::pointer;
    using const_pointer          = typename allocator_traits<allocator_type>::const_pointer;

    using iterator               = wrap_iter<pointer>;
    using const_iterator         = wrap_iter<const_pointer>;

    using reverse_iterator       = ciel::reverse_iterator<iterator>;
    using const_reverse_iterator = ciel::reverse_iterator<const_iterator>;

private:
    using alloc_traits           = allocator_traits<allocator_type>;

    pointer begin_;
    pointer end_;
    pointer end_cap_;
    [[no_unique_address]] allocator_type allocator_;

    constexpr auto alloc_range_destroy(pointer begin, pointer end) noexcept -> pointer {
        CIEL_PRECONDITION(begin <= end);

        while (end != begin) {
            alloc_traits::destroy(allocator_, --end);
        }
        return end;
    }

    template<class... Args>
    constexpr auto alloc_range_construct_n(pointer begin, const size_type n, Args&& ... args) -> pointer {
        pointer end = begin;

        CIEL_TRY {
            for (size_type i = 0; i < n; ++i) {
                alloc_traits::construct(allocator_, end, std::forward<Args>(args)...);
                ++end;
            }
            return end;

        } CIEL_CATCH (...) {
            alloc_range_destroy(begin, end);
            CIEL_THROW;
        }
    }

    template<class Iter>
    constexpr auto alloc_range_construct(pointer begin, Iter first, Iter last) -> pointer {
        pointer end = begin;

        CIEL_TRY {
            while (first != last) {
                alloc_traits::construct(allocator_, end, *first);
                ++first;
                ++end;
            }
            return end;

        } CIEL_CATCH (...) {
            alloc_range_destroy(begin, end);
            CIEL_THROW;
        }
    }

    constexpr auto range_assign_n(pointer begin, const size_type n, const value_type& value) -> void {
        for (size_type i = 0; i < n; ++i) {
            *begin = value;
            ++begin;
        }
    }

    template<class Iter>
    constexpr auto range_assign(pointer begin, Iter first, Iter last) -> void {
        while (first != last) {
            *begin = *first;
            ++first;
            ++begin;
        }
    }

    // Used in expansion
    constexpr auto alloc_range_move(pointer begin, pointer first, pointer last) noexcept -> pointer {
        CIEL_PRECONDITION(first <= last);

        pointer end = begin;
        while (first != last) {
            alloc_traits::construct(allocator_, end++, std::move(*first++));
        }
        return end;
    }

    [[nodiscard]] constexpr auto get_new_allocation(const size_type new_cap) -> allocation_result<pointer, size_type> {
        CIEL_PRECONDITION(new_cap > 0);

        if (new_cap > max_size()) [[unlikely]] {
            THROW(std::length_error("ciel::vector reserving size is beyond max_size"));
        }
        return alloc_traits::allocate_at_least(allocator_, new_cap);
    }

    // move old elements to new_start, destroy old elements, deallocate old memory, renew begin_, end_, end_cap_
    constexpr auto reserve_to(allocation_result<pointer, size_type> new_allocation) noexcept -> void {
        CIEL_PRECONDITION(new_allocation.count > 0);
        CIEL_PRECONDITION(size() <= new_allocation.count);

        pointer new_start = new_allocation.ptr;

        alloc_range_move(new_start, begin_, end_);

        const size_type old_size = size();

        if (begin_) {
            clear();
            alloc_traits::deallocate(allocator_, begin_, capacity());
        }

        begin_ = new_start;
        end_ = begin_ + old_size;
        end_cap_ = begin_ + new_allocation.count;
    }

    template<class... Args>
    constexpr auto insert_n(iterator pos, const size_type count, Args&& ... args) -> iterator {
        if (count == 0) [[unlikely]] {
            return pos;
        }

        // When it comes to expansion, we need to construct new elements directly on new space,
        // if it throws then has no effect. And move construct two ranges divided by pos on new space

        if (const size_type new_size = size() + count; new_size > capacity()) {
            size_type new_cap = capacity() ? capacity() * 2 : 1;
            while (new_size > new_cap) {
                new_cap *= 2;
            }

            const size_type idx = pos - begin();
            pointer new_start = alloc_traits::allocate(allocator_, new_cap);
            pointer new_pos = new_start + idx;

            CIEL_TRY {
                alloc_range_construct_n(new_pos, count, std::forward<Args>(args)...);
            } CIEL_CATCH (...) {
                alloc_traits::deallocate(allocator_, new_start, new_cap);
                CIEL_THROW;
            }

            if (begin_) {
                alloc_range_move(new_start, begin_, to_address(pos));
                alloc_range_move(new_pos + count, to_address(pos), end_);

                clear();
                alloc_traits::deallocate(allocator_, begin_, capacity());
            }

            begin_ = new_start;
            end_ = begin_ + new_size;
            end_cap_ = begin_ + new_cap;

            return iterator(new_pos);
        } else {
            // When capacity is enough, we construct them all at end and rotate them to the right place

            iterator old_end = end();
            end_ = alloc_range_construct_n(end_, count, std::forward<Args>(args)...);
            rotate(pos, old_end, end());
            return pos;
        }
    }

    constexpr auto clear_and_get_cap_no_less_than(const size_type cap) -> void {
        clear();
        if (capacity() < cap) {
            if (begin_) {
                alloc_traits::deallocate(allocator_, begin_, capacity());

                // Prevent exceptions throwing and double free in destructor when asking for memory
                begin_ = nullptr;
                end_ = nullptr;
                end_cap_ = nullptr;
            }
            begin_ = alloc_traits::allocate(allocator_, cap);
            end_cap_ = begin_ + cap;
            end_ = begin_;
        }
    }

public:
    constexpr vector() noexcept(noexcept(allocator_type()))
        : begin_(nullptr), end_(nullptr), end_cap_(nullptr), allocator_() {}

    constexpr explicit vector(const allocator_type& alloc) noexcept
        : begin_(nullptr), end_(nullptr), end_cap_(nullptr), allocator_(alloc) {}

    constexpr vector(const size_type count, const value_type& value, const allocator_type& alloc = allocator_type())
        : vector(alloc) {
        if (count > 0) [[likely]] {
            begin_ = alloc_traits::allocate(allocator_, count);
            end_cap_ = begin_ + count;
            CIEL_TRY {
                end_ = alloc_range_construct_n(begin_, count, value);
            } CIEL_CATCH (...) {
                alloc_traits::deallocate(allocator_, begin_, capacity());
                CIEL_THROW;
            }
        }
    }

    constexpr explicit vector(const size_type count, const allocator_type& alloc = allocator_type())
        : vector(alloc) {
        if (count > 0) [[likely]] {
            begin_ = alloc_traits::allocate(allocator_, count);
            end_cap_ = begin_ + count;
            CIEL_TRY {
                end_ = alloc_range_construct_n(begin_, count);
            } CIEL_CATCH (...) {
                alloc_traits::deallocate(allocator_, begin_, capacity());
                CIEL_THROW;
            }
        }
    }

    template<class Iter>
        requires is_exactly_input_iterator<Iter>::value
    constexpr vector(Iter first, Iter last, const allocator_type& alloc = allocator_type())
        : vector(alloc) {
        CIEL_TRY {
            while (first != last) {
                emplace_back(*first);
                ++first;
            }
        } CIEL_CATCH (...) {
            if (begin_) {
                clear();
                alloc_traits::deallocate(allocator_, begin_, capacity());
            }
            CIEL_THROW;
        }
    }

    template<class Iter>
        requires is_forward_iterator<Iter>::value
    constexpr vector(Iter first, Iter last, const allocator_type& alloc = allocator_type())
        : vector(alloc) {
        if (const auto count = ciel::distance(first, last); count > 0) [[likely]] {
            begin_ = alloc_traits::allocate(allocator_, count);
            end_cap_ = begin_ + count;
            CIEL_TRY {
                end_ = alloc_range_construct(begin_, first, last);
            } CIEL_CATCH (...) {
                alloc_traits::deallocate(allocator_, begin_, capacity());
                CIEL_THROW;
            }
        }
    }

    constexpr vector(const vector& other)
        : vector(other.begin(), other.end(),
                 alloc_traits::select_on_container_copy_construction(other.get_allocator())) {}

    constexpr vector(const vector& other, const allocator_type& alloc)
        : vector(other.begin(), other.end(), alloc) {}

    constexpr vector(vector&& other) noexcept
        : begin_(other.begin_), end_(other.end_), end_cap_(other.end_cap_), allocator_(std::move(other.allocator_)) {
        other.begin_ = nullptr;
        other.end_ = nullptr;
        other.end_cap_ = nullptr;
    }

    constexpr vector(vector&& other, const allocator_type& alloc) {
        if (alloc == other.get_allocator()) {
            allocator_ = alloc;
            begin_ = other.begin_;
            end_ = other.end_;
            end_cap_ = other.end_cap_;
            other.begin_ = nullptr;
            other.end_ = nullptr;
            other.end_cap_ = nullptr;
        } else {
            vector(other, alloc).swap(*this);
        }
    }

    constexpr vector(std::initializer_list<value_type> init, const allocator_type& alloc = allocator_type())
        : vector(init.begin(), init.end(), alloc) {}

    constexpr ~vector() {
        if (begin_) {
            clear();
            alloc_traits::deallocate(allocator_, begin_, capacity());
        }
    }

    constexpr auto operator=(const vector& other) -> vector& {
        if (this == addressof(other)) [[unlikely]] {
            return *this;
        }

        if (alloc_traits::propagate_on_container_copy_assignment::value) {
            if (allocator_ != other.allocator_) {
                vector(other.allocator_).swap(*this);
                assign(other.begin(), other.end());
                return *this;
            }

            allocator_ = other.allocator_;
        }
        assign(other.begin(), other.end());

        CIEL_POSTCONDITION(*this == other);
        return *this;
    }

    constexpr auto operator=(vector&& other)
        noexcept(alloc_traits::propagate_on_container_move_assignment::value ||
                 alloc_traits::is_always_equal::value) -> vector& {
        if (this == addressof(other)) [[unlikely]] {
            return *this;
        }

        if (!alloc_traits::propagate_on_container_move_assignment::value && allocator_ != other.allocator_) {
            assign(other.begin(), other.end());
            return *this;
        }

        if (alloc_traits::propagate_on_container_move_assignment::value) {
            allocator_ = std::move(other.allocator_);
        }
        if (begin_) {
            clear();
            alloc_traits::deallocate(allocator_, begin_, capacity());
        }
        begin_ = other.begin_;
        end_ = other.end_;
        end_cap_ = other.end_cap_;
        other.begin_ = nullptr;
        other.end_ = nullptr;
        other.end_cap_ = nullptr;
        return *this;
    }

    constexpr auto operator=(std::initializer_list<value_type> ilist) -> vector& {
        assign(ilist.begin(), ilist.end());
        return *this;
    }

    constexpr auto assign(const size_type count, const value_type& value) -> void {
        if (capacity() < count) {
            clear_and_get_cap_no_less_than(count);
            end_ = alloc_range_construct_n(begin_, count, value);
            return;
        }

        if (size() > count) {
            end_ = alloc_range_destroy(begin_ + count, end_);
        }

        CIEL_POSTCONDITION(size() <= count);

        range_assign_n(begin_, size(), value);
        // if count > size()
        end_ = alloc_range_construct_n(end_, count - size(), value);

        CIEL_POSTCONDITION(size() == count);
    }

    template<class Iter>
        requires is_forward_iterator<Iter>::value
    constexpr auto assign(Iter first, Iter last) -> void {
        const size_type count = ciel::distance(first, last);

        if (capacity() < count) {
            clear_and_get_cap_no_less_than(count);
            end_ = alloc_range_construct(begin_, first, last);
            return;
        }

        if (size() > count) {
            end_ = alloc_range_destroy(begin_ + count, end_);
        }

        CIEL_POSTCONDITION(size() <= count);

        Iter mid = first + size();

        range_assign(begin_, first, mid);
        // if mid < last
        end_ = alloc_range_construct(end_, mid, last);

        CIEL_POSTCONDITION(size() == count);
    }

    template<class Iter>
        requires is_exactly_input_iterator<Iter>::value
    constexpr auto assign(Iter first, Iter last) -> void {
        clear();
        while (first != last) {
            emplace_back(*first);
            ++first;
        }
    }

    constexpr auto assign(std::initializer_list<value_type> ilist) -> void {
        assign(ilist.begin(), ilist.end());
    }

    [[nodiscard]] constexpr auto get_allocator() const noexcept -> allocator_type {
        return allocator_;
    }

    [[nodiscard]] constexpr auto at(const size_type pos) -> reference {
        if (pos >= size()) [[unlikely]] {
            THROW(std::out_of_range("pos is not within the range of ciel::vector"));
        }
        return begin_[pos];
    }

    [[nodiscard]] constexpr auto at(const size_type pos) const -> const_reference {
        if (pos >= size()) [[unlikely]] {
            THROW(std::out_of_range("pos is not within the range of ciel::vector"));
        }
        return begin_[pos];
    }

    [[nodiscard]] constexpr auto operator[](const size_type pos) -> reference {
        CIEL_PRECONDITION(pos < size());

        return begin_[pos];
    }

    [[nodiscard]] constexpr auto operator[](const size_type pos) const -> const_reference {
        CIEL_PRECONDITION(pos < size());

        return begin_[pos];
    }

    [[nodiscard]] constexpr auto front() -> reference {
        CIEL_PRECONDITION(!empty());

        return begin_[0];
    }

    [[nodiscard]] constexpr auto front() const -> const_reference {
        CIEL_PRECONDITION(!empty());

        return begin_[0];
    }

    [[nodiscard]] constexpr auto back() -> reference {
        CIEL_PRECONDITION(!empty());

        return *(end_ - 1);
    }

    [[nodiscard]] constexpr auto back() const -> const_reference {
        CIEL_PRECONDITION(!empty());

        return *(end_ - 1);
    }

    [[nodiscard]] constexpr auto data() noexcept -> T* {
        return begin_;
    }

    [[nodiscard]] constexpr auto data() const noexcept -> const T* {
        return begin_;
    }

    [[nodiscard]] constexpr auto begin() noexcept -> iterator {
        return iterator(begin_);
    }

    [[nodiscard]] constexpr auto begin() const noexcept -> const_iterator {
        return const_iterator(begin_);
    }

    [[nodiscard]] constexpr auto cbegin() const noexcept -> const_iterator {
        return begin();
    }

    [[nodiscard]] constexpr auto end() noexcept -> iterator {
        return iterator(end_);
    }

    [[nodiscard]] constexpr auto end() const noexcept -> const_iterator {
        return const_iterator(end_);
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

    [[nodiscard]] constexpr auto empty() const noexcept -> bool {
        return begin_ == end_;
    }

    [[nodiscard]] constexpr auto size() const noexcept -> size_type {
        return end_ - begin_;
    }

    [[nodiscard]] constexpr auto max_size() const noexcept -> size_type {
        return alloc_traits::max_size(allocator_);
    }

    // Separate reserve into two steps to be used by emplace_back
    // Consider this situation: v.emplace_back(v[0]);
    // So we need to construct new elements on new space first,
    // then move old elements to new space and clear old space
    constexpr auto reserve(const size_type new_cap) -> void {
        if (new_cap <= capacity()) {
            return;
        }
        auto new_allocation = get_new_allocation(new_cap);
        reserve_to(new_allocation);
    }

    [[nodiscard]] constexpr auto capacity() const noexcept -> size_type {
        return end_cap_ - begin_;
    }

    constexpr auto shrink_to_fit() -> void {
        if (size() == capacity()) [[unlikely]] {
            return;
        }

        if (size() > 0) {
            pointer new_start = alloc_traits::allocate(allocator_, size());
            reserve_to({new_start, size()});

        } else {
            alloc_traits::deallocate(allocator_, begin_, capacity());
            begin_ = nullptr;
            end_ = nullptr;
            end_cap_ = nullptr;
        }
    }

    constexpr auto clear() noexcept -> void {
        end_ = alloc_range_destroy(begin_, end_);
    }

    constexpr auto insert(iterator pos, const value_type& value) -> iterator {
        return insert_n(pos, 1, value);
    }

    constexpr auto insert(iterator pos, value_type&& value) -> iterator {
        return insert_n(pos, 1, std::move(value));
    }

    constexpr auto insert(iterator pos, const size_type count, const value_type& value) -> iterator {
        return insert_n(pos, count, value);
    }

    // We construct all at the end at first, then rotate them to the right place
    template<class Iter>
        requires is_exactly_input_iterator<Iter>::value
    constexpr auto insert(iterator pos, Iter first, Iter last) -> iterator {
        // record these index because it may reallocate
        const auto pos_index = pos - begin();
        const size_type old_size = size();

        CIEL_TRY {
            while (first != last) {
                emplace_back(*first);
                ++first;
            }
        } CIEL_CATCH (...) {
            end_ = alloc_range_destroy(begin() + old_size, end());
            CIEL_THROW;
        }

        rotate(begin() + pos_index, begin() + old_size, end());
        return begin() += pos_index;
    }

    template<class Iter>
        requires is_forward_iterator<Iter>::value
    constexpr auto insert(iterator pos, Iter first, Iter last) -> iterator {
        const auto count = ciel::distance(first, last);
        if (count <= 0) [[unlikely]] {
            return pos;
        }

        if (const size_type new_size = size() + count; new_size > capacity()) {
            size_type new_cap = capacity() ? capacity() * 2 : 1;
            while (new_size > new_cap) {
                new_cap *= 2;
            }

            size_type idx = pos - begin();
            pointer new_start = alloc_traits::allocate(allocator_, new_cap);
            pointer new_pos = new_start + idx;

            CIEL_TRY {
                alloc_range_construct(new_pos, first, last);
            } CIEL_CATCH (...) {
                alloc_traits::deallocate(allocator_, new_start, new_cap);
                CIEL_THROW;
            }

            if (begin_) {
                alloc_range_move(new_start, begin_, to_address(pos));
                alloc_range_move(new_pos + count, to_address(pos), end_);

                clear();
                alloc_traits::deallocate(allocator_, begin_, capacity());
            }

            begin_ = new_start;
            end_ = begin_ + new_size;
            end_cap_ = begin_ + new_cap;

            return iterator(new_pos);
        } else {
            iterator old_end = end();
            end_ = alloc_range_construct(end_, first, last);
            rotate(pos, old_end, end());
            return pos;
        }
    }

    constexpr auto insert(iterator pos, std::initializer_list<value_type> ilist) -> iterator {
        return insert(pos, ilist.begin(), ilist.end());
    }

    template<class... Args>
    constexpr auto emplace(iterator pos, Args&& ... args) -> iterator {
        if (pos == end()) {
            emplace_back(std::forward<Args>(args)...);
            return iterator(end_ - 1);    // There is possiblity of expansion, don't return pos
        }

        return insert_n(pos, 1, std::forward<Args>(args)...);
    }

    constexpr auto erase(iterator pos) noexcept -> iterator {
        CIEL_PRECONDITION(!empty());

        return erase(pos, pos + 1);
    }

    constexpr auto erase(iterator first, iterator last) noexcept -> iterator {
        if (auto distance = ciel::distance(first, last); distance <= 0) [[unlikely]] {
            return last;
        }

        auto index = first - begin();

        iterator new_end = move(last, end(), first);
        end_ = alloc_range_destroy(to_address(new_end), end_);

        return begin() + index;
    }

    constexpr auto push_back(const value_type& value) -> void {
        emplace_back(value);
    }

    constexpr auto push_back(value_type&& value) -> void {
        emplace_back(std::move(value));
    }

    template<class... Args>
    constexpr auto emplace_back(Args&& ... args) -> reference {
        if (size_type new_cap = capacity(); size() == new_cap) {
            new_cap = new_cap ? 2 * new_cap : 1;

            auto new_allocation = get_new_allocation(new_cap);
            pointer new_start = new_allocation.ptr;
            new_cap = new_allocation.count;

            CIEL_TRY {
                alloc_range_construct_n(new_start + size(), 1, std::forward<Args>(args)...);
            } CIEL_CATCH (...) {
                alloc_traits::deallocate(allocator_, new_start, new_cap);
                CIEL_THROW;
            }
            reserve_to(new_allocation);
            ++end_;
        } else {
            end_ = alloc_range_construct_n(end_, 1, std::forward<Args>(args)...);
        }
        return back();
    }

    constexpr auto pop_back() noexcept -> void {
        CIEL_PRECONDITION(!empty());

        end_ = alloc_range_destroy(end_ - 1, end_);
    }

    constexpr auto resize(const size_type count) -> void {
        if (size() >= count) {
            end_ = alloc_range_destroy(begin_ + count, end_);
            return;
        }

        if (count > capacity()) {
            reserve(count);
        }
        end_ = alloc_range_construct_n(end_, count - size());
    }

    constexpr auto resize(const size_type count, const value_type& value) -> void {
        if (size() >= count) {
            end_ = alloc_range_destroy(begin_ + count, end_);
            return;
        }

        if (count > capacity()) {
            reserve(count);
        }
        end_ = alloc_range_construct_n(end_, count - size(), value);
    }

    constexpr auto swap(vector& other) noexcept(alloc_traits::propagate_on_container_swap::value ||
                                                alloc_traits::is_always_equal::value) -> void {
        using std::swap;
        swap(begin_, other.begin_);
        swap(end_, other.end_);
        swap(end_cap_, other.end_cap_);
        swap(allocator_, other.allocator_);
    }

};  // class vector

template<class T, class Alloc>
[[nodiscard]] constexpr auto operator==(const vector<T, Alloc>& lhs, const vector<T, Alloc>& rhs) -> bool {
    if (lhs.size() != rhs.size()) {
        return false;
    }
    return equal(lhs.begin(), lhs.end(), rhs.begin());
}

// So that we can test more efficiently
template<class T, class Alloc>
[[nodiscard]] constexpr auto operator==(const vector<T, Alloc>& lhs, std::initializer_list<T> rhs) -> bool {
    if (lhs.size() != rhs.size()) {
        return false;
    }
    return ciel::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<class T, class Alloc, class U>
constexpr auto erase(vector<T, Alloc>& c, const U& value) -> typename vector<T, Alloc>::size_type {
    auto it = ciel::remove(c.begin(), c.end(), value);
    auto r = distance(it, c.end());
    c.erase(it, c.end());
    return r;
}

template<class T, class Alloc, class Pred>
constexpr auto erase_if(vector<T, Alloc>& c, Pred pred) -> typename vector<T, Alloc>::size_type {
    auto it = ciel::remove_if(c.begin(), c.end(), pred);
    auto r = distance(it, c.end());
    c.erase(it, c.end());
    return r;
}

template<class Iter, class Alloc = allocator<typename iterator_traits<Iter>::value_type>>
vector(Iter, Iter, Alloc = Alloc()) -> vector<typename iterator_traits<Iter>::value_type, Alloc>;

NAMESPACE_CIEL_END

namespace std {

template<class T, class Alloc>
constexpr auto swap(ciel::vector<T, Alloc>& lhs,
                    ciel::vector<T, Alloc>& rhs) noexcept(noexcept(lhs.swap(rhs))) -> void {
    lhs.swap(rhs);
}

}   // namespace std

#endif // CIELUTILS_INCLUDE_CIEL_VECTOR_HPP_