#ifndef CIELUTILS_INCLUDE_CIEL_CIRCULAR_BUFFER_HPP_
#define CIELUTILS_INCLUDE_CIEL_CIRCULAR_BUFFER_HPP_

#include <ciel/algorithm_impl/equal.hpp>
#include <ciel/algorithm_impl/max.hpp>
#include <ciel/algorithm_impl/move.hpp>
#include <ciel/algorithm_impl/move_backward.hpp>
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

template<class T, class Pointer, class Reference>
class circular_buffer_iterator {
public:
    using difference_type   = ptrdiff_t;
    using value_type        = T;
    using pointer           = Pointer;
    using reference         = Reference;
    using iterator_category = random_access_iterator_tag;
    using iterator_concept  = random_access_iterator_tag;

private:
    pointer cur_;
    pointer begin_cap_;
    pointer end_cap_;   // cur_ != end_cap_
    pointer begin_;     // We need it to judge the relative position of two iterators when subtracting

    [[nodiscard]] auto capacity() const noexcept -> difference_type {
        return end_cap_ - begin_cap_;
    }

    template<class, class, class>
    friend class circular_buffer_iterator;

    template<class U, class Pointer1, class Pointer2, class Reference1, class Reference2>
    friend auto operator-(const circular_buffer_iterator<U, Pointer1, Reference1>& lhs,
                          const circular_buffer_iterator<U, Pointer2, Reference2>& rhs) noexcept
        -> typename iterator_traits<circular_buffer_iterator<U, Pointer1, Reference1>>::difference_type;

public:
    circular_buffer_iterator() noexcept = default;

    circular_buffer_iterator(pointer c, pointer bc, pointer ec, pointer b) noexcept
        : cur_(c), begin_cap_(bc), end_cap_(ec), begin_(b) {}

    circular_buffer_iterator(const circular_buffer_iterator&) noexcept = default;
    circular_buffer_iterator(circular_buffer_iterator&&) noexcept = default;

    // TODO: const_cast
    template<class P, class R>
    circular_buffer_iterator(const circular_buffer_iterator<T, P, R>& other) noexcept
        : cur_(other.cur_), begin_cap_(other.begin_cap_), end_cap_(other.end_cap_), begin_(other.begin_) {}

    ~circular_buffer_iterator() = default;

    auto operator=(const circular_buffer_iterator&) noexcept -> circular_buffer_iterator& = default;
    auto operator=(circular_buffer_iterator&&) noexcept -> circular_buffer_iterator& = default;

    [[nodiscard]] auto operator->() const noexcept -> pointer {
        return cur_;
    }

    [[nodiscard]] auto operator*() const noexcept -> reference {
        return *cur_;
    }

    auto operator++() noexcept -> circular_buffer_iterator& {
        CIEL_PRECONDITION(cur_ != end_cap_);

        if (++cur_ == end_cap_) {
            cur_ = begin_cap_;
        }
        return *this;
    }

    [[nodiscard]] auto operator++(int) noexcept -> circular_buffer_iterator {
        circular_buffer_iterator res(*this);
        ++(*this);
        return res;
    }

    auto operator--() noexcept -> circular_buffer_iterator& {
        if (cur_ == begin_cap_) {
            cur_ = end_cap_ - 1;
        } else {
            --cur_;
        }
        return *this;
    }

    [[nodiscard]] auto operator--(int) noexcept -> circular_buffer_iterator {
        circular_buffer_iterator res(*this);
        --(*this);
        return res;
    }

    auto operator+=(const difference_type n) noexcept -> circular_buffer_iterator& {
        CIEL_PRECONDITION(std::abs(n) <= capacity());

        cur_ += n;

        if (cur_ >= end_cap_) {
            cur_ -= capacity();

        } else if (cur_ < begin_cap_) {
            cur_ += capacity();
        }

        CIEL_POSTCONDITION(begin_cap_ <= cur_);
        CIEL_POSTCONDITION(cur_ < end_cap_ || cur_ == nullptr);

        return *this;
    }

    auto operator-=(const difference_type n) noexcept -> circular_buffer_iterator& {
        return (*this) += (-n);
    }

    [[nodiscard]] auto operator+(const difference_type n) const noexcept -> circular_buffer_iterator {
        circular_buffer_iterator res(*this);
        return res += n;
    }

    [[nodiscard]] auto operator-(const difference_type n) const noexcept -> circular_buffer_iterator {
        circular_buffer_iterator res(*this);
        return res -= n;
    }

    [[nodiscard]] auto base() const noexcept -> pointer {
        return cur_;
    }

    [[nodiscard]] explicit operator bool() const noexcept {
        return base() != nullptr;
    }

};    // class circular_buffer_iterator

template<class T, class Pointer1, class Pointer2, class Reference1, class Reference2>
[[nodiscard]] auto operator==(const circular_buffer_iterator<T, Pointer1, Reference1>& lhs,
                              const circular_buffer_iterator<T, Pointer2, Reference2>& rhs) noexcept -> bool {
    return lhs.base() == rhs.base();
}

template<class T, class Pointer1, class Pointer2, class Reference1, class Reference2>
[[nodiscard]] auto operator!=(const circular_buffer_iterator<T, Pointer1, Reference1>& lhs,
                              const circular_buffer_iterator<T, Pointer2, Reference2>& rhs) noexcept -> bool {
    return !(lhs == rhs);
}

template<class T, class Pointer1, class Pointer2, class Reference1, class Reference2>
[[nodiscard]] auto operator-(const circular_buffer_iterator<T, Pointer1, Reference1>& lhs,
                             const circular_buffer_iterator<T, Pointer2, Reference2>& rhs) noexcept
    -> typename iterator_traits<circular_buffer_iterator<T, Pointer1, Reference1>>::difference_type {
    CIEL_PRECONDITION((!!(lhs) ^ !!(rhs)) == 0);
    CIEL_PRECONDITION(lhs.capacity() == rhs.capacity());
    CIEL_PRECONDITION(lhs.begin_ == rhs.begin_);

    const auto begin = lhs.begin_;

    if (lhs.cur_ >= begin) {
        if (rhs.cur_ >= begin) {
            return lhs.cur_ - rhs.cur_;
        }

        return lhs.cur_ - rhs.cur_ - lhs.capacity();
    }

    if (rhs.cur_ >= begin) {
        return lhs.cur_ - rhs.cur_ + lhs.capacity();
    }

    return lhs.cur_ - rhs.cur_;

    // both >= begin_
    // return lhs.cur_ - rhs.cur_;

    // both < begin_
    // return lhs.cur_ - rhs.cur_;

    // lhs < begin_, rhs >= begin_
    // return lhs.cur_ - rhs.cur_ + lhs.capacity();

    // lhs >= begin_, rhs < begin_
    // return lhs.cur_ - rhs.cur_ - lhs.capacity();
}

template<class T, class Allocator = allocator<T>>
class circular_buffer {

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

    using iterator               = circular_buffer_iterator<T, pointer, reference>;
    using const_iterator         = circular_buffer_iterator<T, const_pointer, const_reference>;

    using reverse_iterator       = ciel::reverse_iterator<iterator>;
    using const_reverse_iterator = ciel::reverse_iterator<const_iterator>;

private:
    using alloc_traits           = allocator_traits<allocator_type>;

    pointer begin_cap_;
    pointer begin_;
    pointer end_;
    pointer end_cap_;
    [[no_unique_address]] allocator_type allocator_;

    constexpr auto alloc_range_destroy(iterator begin, iterator end) noexcept -> iterator {
        // CIEL_PRECONDITION(end - begin >= 0);

        while (end != begin) {
            alloc_traits::destroy(allocator_, to_address(--end));
        }
        return end;
    }

    template<class... Args>
    constexpr auto alloc_range_construct_n(iterator begin, const size_type n, Args&& ... args) -> iterator {
        iterator end = begin;

        CIEL_TRY {
            for (size_type i = 0; i < n; ++i) {
                alloc_traits::construct(allocator_, to_address(end), std::forward<Args>(args)...);
                ++end;
            }
            return end;

        } CIEL_CATCH (...) {
            alloc_range_destroy(begin, end);
            CIEL_THROW;
        }
    }

    template<class Iter>
    constexpr auto alloc_range_construct(iterator begin, Iter first, Iter last) -> iterator {
        iterator end = begin;

        CIEL_TRY {
            while (first != last) {
                alloc_traits::construct(allocator_, to_address(end), *first);
                ++first;
                ++end;
            }
            return end;

        } CIEL_CATCH (...) {
            alloc_range_destroy(begin, end);
            CIEL_THROW;
        }
    }

    constexpr auto range_assign_n(iterator begin, const size_type n, const value_type& value) -> void {
        for (size_type i = 0; i < n; ++i) {
            *begin = value;
            ++begin;
        }
    }

    template<class Iter>
    constexpr auto range_assign(iterator begin, Iter first, Iter last) -> void {
        while (first != last) {
            *begin = *first;
            ++first;
            ++begin;
        }
    }

    // Used in expansion
    constexpr auto alloc_range_move(pointer begin, iterator first, iterator last) noexcept -> pointer {
        CIEL_PRECONDITION(last - first >= 0);

        pointer end = begin;
        while (first != last) {
            alloc_traits::construct(allocator_, end++, std::move(*first++));
        }
        return end;
    }

    [[nodiscard]] constexpr auto real_capacity() const noexcept -> size_type {
        return end_cap_ - begin_cap_;
    }

    [[nodiscard]] auto spare() const noexcept -> size_type {
        return capacity() - size();
    }


    [[nodiscard]] constexpr auto get_new_allocation(const size_type new_cap) -> allocation_result<pointer, size_type> {
        CIEL_PRECONDITION(new_cap > 1);

        if (new_cap > max_size()) [[unlikely]] {
            THROW(std::length_error("ciel::circular_buffer reserving size is beyond max_size"));
        }
        return alloc_traits::allocate_at_least(allocator_, new_cap);
    }

    // move old elements to new_start, destroy old elements, deallocate old memory,
    // renew begin_cap_, begin_, end_, end_cap_
    constexpr auto reserve_to(allocation_result<pointer, size_type> new_allocation) noexcept -> void {
        CIEL_PRECONDITION(new_allocation.count > size());

        pointer new_start = new_allocation.ptr;

        alloc_range_move(new_start, begin(), end());

        const size_type old_size = size();

        if (begin_cap_) {
            clear();
            alloc_traits::deallocate(allocator_, begin_cap_, real_capacity());
        }

        begin_cap_ = new_start;
        begin_ = new_start;
        end_ = begin_ + old_size;
        end_cap_ = begin_cap_ + new_allocation.count;
    }

    // Check which endpoint is pos close to
    template<class... Args>
    auto insert_n(iterator pos, const size_type count, Args&& ... args) -> iterator {
        if (count == 0) [[unlikely]] {
            return pos;
        }

        const size_type pos_index = pos - begin();

        if (spare() < count) {
            // When it comes to expansion, we need to construct new elements directly on new space,
            // if it throws then has no effect. And move construct two ranges divided by pos on new space

            const size_type new_size = size() + count;
            size_type new_cap = real_capacity() ? real_capacity() * 3 : 2;
            while (new_size >= new_cap) {
                new_cap *= 3;
            }

            pointer new_start = alloc_traits::allocate(allocator_, new_cap);
            pointer new_pos = new_start + pos_index;

            CIEL_TRY {
                alloc_range_construct_n(iterator(new_pos, new_start, new_start + new_cap, new_start), count,
                                        std::forward<Args>(args)...);
            } CIEL_CATCH (...) {
                alloc_traits::deallocate(allocator_, new_start, new_cap);
                CIEL_THROW;
            }

            if (begin_cap_) {
                alloc_range_move(new_start, begin(), pos);
                alloc_range_move(new_pos + count, pos, end());

                clear();
                alloc_traits::deallocate(allocator_, begin_cap_, real_capacity());
            }

            begin_cap_ = new_start;
            begin_ = new_start;
            end_ = begin_ + new_size;
            end_cap_ = begin_cap_ + new_cap;

            return begin() += pos_index;
        }

        if (distance(begin(), pos) < distance(pos, end())) {
            // move left half to left

            alloc_range_construct_n(begin() - count, count, std::forward<Args>(args)...);

            iterator old_begin = begin();
            begin_ = (begin() - count).base();

            rotate(begin(), old_begin, pos);
            return begin() += pos_index;

        } else {    // move right half to right
            iterator old_end = end();
            end_ = alloc_range_construct_n(end(), count, std::forward<Args>(args)...).base();

            rotate(pos, old_end, end());
            return begin() += pos_index;
        }
    }

    constexpr auto clear_and_get_cap_no_less_than(size_type cap) -> void {
        clear();
        if (real_capacity() <= cap) {
            if (begin_cap_) {
                alloc_traits::deallocate(allocator_, begin_cap_, real_capacity());

                // Prevent exceptions throwing and double free in destructor when asking for memory
                begin_cap_ = nullptr;
                end_cap_ = nullptr;
                begin_ = nullptr;
                end_ = nullptr;
            }
            begin_cap_ = alloc_traits::allocate(allocator_, cap + 1);
            end_cap_ = begin_cap_ + cap + 1;
            begin_ = begin_cap_;
            end_ = begin_;
        }
    }

public:
    constexpr circular_buffer() noexcept(noexcept(allocator_type()))
        : begin_cap_(nullptr), begin_(nullptr), end_(nullptr), end_cap_(nullptr), allocator_() {}

    constexpr explicit circular_buffer(const allocator_type& alloc) noexcept
        : begin_cap_(nullptr), begin_(nullptr), end_(nullptr), end_cap_(nullptr), allocator_(alloc) {}

    constexpr circular_buffer(const size_type count, const value_type& value,
                              const allocator_type& alloc = allocator_type())
        : circular_buffer(alloc) {
        if (count > 0) [[likely]] {
            begin_cap_ = alloc_traits::allocate(allocator_, count + 1);
            end_cap_ = begin_cap_ + count + 1;

            begin_ = begin_cap_;

            CIEL_TRY {
                end_ = alloc_range_construct_n(begin(), count, value).base();
            } CIEL_CATCH (...) {
                alloc_traits::deallocate(allocator_, begin_cap_, real_capacity());
                CIEL_THROW;
            }
        }
    }

    constexpr explicit circular_buffer(const size_type count, const allocator_type& alloc = allocator_type())
        : circular_buffer(alloc) {
        if (count > 0) [[likely]] {
            begin_cap_ = alloc_traits::allocate(allocator_, count + 1);
            end_cap_ = begin_cap_ + count + 1;

            begin_ = begin_cap_;

            CIEL_TRY {
                end_ = alloc_range_construct_n(begin(), count).base();
            } CIEL_CATCH (...) {
                alloc_traits::deallocate(allocator_, begin_cap_, real_capacity());
                CIEL_THROW;
            }
        }
    }

    template<class Iter>
        requires is_exactly_input_iterator<Iter>::value
    constexpr circular_buffer(Iter first, Iter last, const allocator_type& alloc = allocator_type())
        : circular_buffer(alloc) {
        CIEL_TRY {
            while (first != last) {
                emplace_back(*first);
                ++first;
            }
        } CIEL_CATCH (...) {
            clear();
            alloc_traits::deallocate(allocator_, begin_cap_, real_capacity());
            CIEL_THROW;
        }
    }

    template<class Iter>
        requires is_forward_iterator<Iter>::value
    constexpr circular_buffer(Iter first, Iter last, const allocator_type& alloc = allocator_type())
        : circular_buffer(alloc) {
        if (auto count = ciel::distance(first, last); count > 0) [[likely]] {
            begin_cap_ = alloc_traits::allocate(allocator_, count + 1);
            end_cap_ = begin_cap_ + count + 1;

            begin_ = begin_cap_;

            CIEL_TRY {
                end_ = alloc_range_construct(begin(), first, last).base();
            } CIEL_CATCH (...) {
                alloc_traits::deallocate(allocator_, begin_cap_, real_capacity());
                CIEL_THROW;
            }
        }
    }

    constexpr circular_buffer(const circular_buffer& other)
        : circular_buffer(other.begin(), other.end(),
                 alloc_traits::select_on_container_copy_construction(other.get_allocator())) {}

    constexpr circular_buffer(const circular_buffer& other, const allocator_type& alloc)
        : circular_buffer(other.begin(), other.end(), alloc) {}

    constexpr circular_buffer(circular_buffer&& other) noexcept
        : begin_cap_(other.begin_cap_), begin_(other.begin_), end_(other.end_), end_cap_(other.end_cap_),
          allocator_(std::move(other.allocator_)) {
        other.begin_cap_ = nullptr;
        other.begin_ = nullptr;
        other.end_ = nullptr;
        other.end_cap_ = nullptr;
    }

    constexpr circular_buffer(circular_buffer&& other, const allocator_type& alloc) {
        if (alloc == other.get_allocator()) {
            allocator_ = alloc;
            begin_cap_ = other.begin_cap_;
            begin_ = other.begin_;
            end_ = other.end_;
            end_cap_ = other.end_cap_;
            other.begin_cap_ = nullptr;
            other.begin_ = nullptr;
            other.end_ = nullptr;
            other.end_cap_ = nullptr;
        } else {
            circular_buffer(other, alloc).swap(*this);
        }
    }

    constexpr circular_buffer(std::initializer_list<value_type> init, const allocator_type& alloc = allocator_type())
        : circular_buffer(init.begin(), init.end(), alloc) {}

    constexpr ~circular_buffer() {
        if (begin_cap_) {
            clear();
            alloc_traits::deallocate(allocator_, begin_cap_, real_capacity());
        }
    }

    constexpr auto operator=(const circular_buffer& other) -> circular_buffer& {
        if (this == addressof(other)) [[unlikely]] {
            return *this;
        }

        if (alloc_traits::propagate_on_container_copy_assignment::value) {
            if (allocator_ != other.allocator_) {
                circular_buffer(other.allocator_).swap(*this);
                assign(other.begin(), other.end());
                return *this;
            }

            allocator_ = other.allocator_;
        }
        assign(other.begin(), other.end());
        return *this;
    }

    constexpr auto operator=(circular_buffer&& other)
        noexcept(alloc_traits::propagate_on_container_move_assignment::value ||
                 alloc_traits::is_always_equal::value) -> circular_buffer& {
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
        if (begin_cap_) {
            clear();
            alloc_traits::deallocate(allocator_, begin_cap_, real_capacity());
        }
        begin_cap_ = other.begin_cap_;
        begin_ = other.begin_;
        end_ = other.end_;
        end_cap_ = other.end_cap_;
        other.begin_cap_ = nullptr;
        other.begin_ = nullptr;
        other.end_ = nullptr;
        other.end_cap_ = nullptr;
        return *this;
    }

    constexpr auto operator=(std::initializer_list<value_type> ilist) -> circular_buffer& {
        assign(ilist.begin(), ilist.end());
        return *this;
    }

    constexpr auto assign(const size_type count, const value_type& value) -> void {
        if (real_capacity() <= count) {
            clear_and_get_cap_no_less_than(count);
            end_ = alloc_range_construct_n(begin(), count, value).base();
            return;
        }

        if (size() > count) {
            end_ = alloc_range_destroy(begin() + count, end()).base();
        }

        CIEL_POSTCONDITION(size() <= count);

        range_assign_n(begin(), size(), value);
        // if count > size()
        end_ = alloc_range_construct_n(end(), count - size(), value).base();

        CIEL_POSTCONDITION(size() == count);
    }

    template<class Iter>
        requires is_forward_iterator<Iter>::value
    constexpr auto assign(Iter first, Iter last) -> void {
        const size_type count = ciel::distance(first, last);

        if (real_capacity() <= count) {
            clear_and_get_cap_no_less_than(count);
            end_ = alloc_range_construct(begin(), first, last).base();
            return;
        }

        if (size() > count) {
            end_ = alloc_range_destroy(begin() + count, end()).base();
        }

        CIEL_POSTCONDITION(size() <= count);

        Iter mid = first + size();

        range_assign(begin(), first, mid);
        // if mid < last
        end_ = alloc_range_construct(end(), mid, last).base();

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
        return *(begin() + pos);
    }

    [[nodiscard]] constexpr auto at(const size_type pos) const -> const_reference {
        if (pos >= size()) [[unlikely]] {
            THROW(std::out_of_range("pos is not within the range of ciel::vector"));
        }
        return *(begin() + pos);
    }

    [[nodiscard]] constexpr auto operator[](const size_type pos) -> reference {
        CIEL_PRECONDITION(pos < size());

        return *(begin() + pos);
    }

    [[nodiscard]] constexpr auto operator[](const size_type pos) const -> const_reference {
        CIEL_PRECONDITION(pos < size());

        return *(begin() + pos);
    }

    [[nodiscard]] constexpr auto front() -> reference {
        CIEL_PRECONDITION(!empty());

        return *begin_;
    }

    [[nodiscard]] constexpr auto front() const -> const_reference {
        CIEL_PRECONDITION(!empty());

        return *begin_;
    }

    [[nodiscard]] constexpr auto back() -> reference {
        CIEL_PRECONDITION(!empty());

        return *(--end());
    }

    [[nodiscard]] constexpr auto back() const -> const_reference {
        CIEL_PRECONDITION(!empty());

        return *(--end());
    }

    [[nodiscard]] constexpr auto begin() noexcept -> iterator {
        return iterator(begin_, begin_cap_, end_cap_, begin_);
    }

    [[nodiscard]] constexpr auto begin() const noexcept -> const_iterator {
        return const_iterator(begin_, begin_cap_, end_cap_, begin_);
    }

    [[nodiscard]] constexpr auto cbegin() const noexcept -> const_iterator {
        return begin();
    }

    [[nodiscard]] constexpr auto end() noexcept -> iterator {
        return iterator(end_, begin_cap_, end_cap_, begin_);
    }

    [[nodiscard]] constexpr auto end() const noexcept -> const_iterator {
        return const_iterator(end_, begin_cap_, end_cap_, begin_);
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
        return end() - begin();
    }

    [[nodiscard]] constexpr auto max_size() const noexcept -> size_type {
        return alloc_traits::max_size(allocator_);
    }

    constexpr auto reserve(const size_type new_cap) -> void {
        if (new_cap < real_capacity()) {
            return;
        }
        auto new_allocation = get_new_allocation(new_cap + 1);
        reserve_to(new_allocation);
    }

    // We need end_ + 1 == begin_ to illustrate it's full
    // So the block of memory that begin_cap_ holds must be no less than 2
    [[nodiscard]] constexpr auto capacity() const noexcept -> size_type {
        return begin_cap_ ? real_capacity() - 1 : 0;
    }

    constexpr auto shrink_to_fit() -> void {
        if (size() == capacity()) [[unlikely]] {
            return;
        }

        if (size() > 0) {
            pointer new_start = alloc_traits::allocate(allocator_, size() + 1);
            reserve_to({new_start, size() + 1});

        } else {
            alloc_traits::deallocate(allocator_, begin_cap_, real_capacity());
            begin_cap_ = nullptr;
            begin_ = nullptr;
            end_ = nullptr;
            end_cap_ = nullptr;
        }
    }

    constexpr auto clear() noexcept -> void {
        end_ = alloc_range_destroy(begin(), end()).base();
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
            end_ = alloc_range_destroy(begin() + old_size, end()).base();
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

        const size_type pos_index = pos - begin();

        if (spare() < static_cast<size_type>(count)) {
            // When it comes to expansion, we need to construct new elements directly on new space,
            // if it throws then has no effect. And move construct two ranges divided by pos on new space

            const size_type new_size = size() + count;
            size_type new_cap = real_capacity() ? real_capacity() * 3 : 2;
            while (new_size >= new_cap) {
                new_cap *= 3;
            }

            pointer new_start = alloc_traits::allocate(allocator_, new_cap);
            pointer new_pos = new_start + pos_index;

            CIEL_TRY {
                alloc_range_construct(iterator(new_pos, new_start, new_start + new_cap, new_start), first, last);
            } CIEL_CATCH (...) {
                alloc_traits::deallocate(allocator_, new_start, new_cap);
                CIEL_THROW;
            }

            if (begin_cap_) {
                alloc_range_move(new_start, begin(), pos);
                alloc_range_move(new_pos + count, pos, end());

                clear();
                alloc_traits::deallocate(allocator_, begin_cap_, real_capacity());
            }

            begin_cap_ = new_start;
            begin_ = new_start;
            end_ = begin_ + new_size;
            end_cap_ = begin_cap_ + new_cap;

            return begin() += pos_index;
        }

        if (distance(begin(), pos) < distance(pos, end())) {
            // move left half to left

            alloc_range_construct(begin() - count, first, last);

            iterator old_begin = begin();
            begin_ = (begin() - count).base();

            rotate(begin(), old_begin, pos);
            return begin() += pos_index;

        } else {    // move right half to right
            iterator old_end = end();
            end_ = alloc_range_construct(end(), first, last).base();

            rotate(pos, old_end, end());
            return begin() += pos_index;
        }
    }

    constexpr auto insert(iterator pos, std::initializer_list<value_type> ilist) -> iterator {
        return insert(pos, ilist.begin(), ilist.end());
    }

    template<class... Args>
    constexpr auto emplace(iterator pos, Args&& ... args) -> iterator {
        if (pos == end()) {
            emplace_back(std::forward<Args>(args)...);
            return --end();    // There is possiblity of expansion, don't return iterator(pos)
        }

        if (pos == begin()) {
            emplace_front(std::forward<Args>(args)...);
            return begin();
        }

        return insert_n(pos, 1, std::forward<Args>(args)...);
    }

    constexpr auto erase(iterator pos) noexcept -> iterator {
        CIEL_PRECONDITION(!empty());

        return erase(pos, pos + 1);
    }

    constexpr auto erase(iterator first, iterator last) noexcept -> iterator {
        if (distance(first, last) <= 0) [[unlikely]] {
            return last;
        }

        const difference_type begin_first_distance = distance(begin(), first);
        CIEL_PRECONDITION(begin_first_distance >= 0);

        if (begin_first_distance < distance(last, end())) {
            iterator old_begin = begin();

            begin_ = move_backward(begin(), first, last).base();

            alloc_range_destroy(old_begin, begin());

        } else {
            iterator new_end = move(last, end(), first);

            end_ = alloc_range_destroy(new_end, end()).base();
        }

        return begin() + begin_first_distance;
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
            new_cap = new_cap ? 3 * new_cap : 2;

            auto new_allocation = get_new_allocation(new_cap);
            pointer new_start = new_allocation.ptr;
            new_cap = new_allocation.count;

            CIEL_TRY {
                alloc_range_construct_n(iterator(new_start, new_start, new_start + new_cap, new_start) + size(), 1,
                                        std::forward<Args>(args)...);
            } CIEL_CATCH (...) {
                alloc_traits::deallocate(allocator_, new_start, new_cap);
                CIEL_THROW;
            }
            reserve_to(new_allocation);
            // end_ = (++end()).base();
            ++end_;     // end_ won't get to end_cap_
        } else {
            end_ = alloc_range_construct_n(end(), 1, std::forward<Args>(args)...).base();
        }
        return back();
    }

    constexpr auto pop_back() noexcept -> void {
        CIEL_PRECONDITION(!empty());

        end_ = alloc_range_destroy(--end(), end()).base();
    }

    constexpr auto push_front(const value_type& value) -> void {
        emplace_front(value);
    }

    constexpr auto push_front(value_type&& value) -> void {
        emplace_front(std::move(value));
    }

    template<class... Args>
    constexpr auto emplace_front(Args&& ... args) -> reference {
        if (size_type new_cap = capacity(); size() == new_cap) {
            new_cap = new_cap ? 3 * new_cap : 2;

            auto new_allocation = get_new_allocation(new_cap);
            pointer new_start = new_allocation.ptr;
            new_cap = new_allocation.count;

            CIEL_TRY {
                // construct at the end of new space so that begin can decrement to that
                alloc_range_construct_n(iterator(new_start, new_start, new_start + new_cap, new_start) + (new_cap - 1), 1, std::forward<Args>(args)...);
            } CIEL_CATCH (...) {
                alloc_traits::deallocate(allocator_, new_start, new_cap);
                CIEL_THROW;
            }
            reserve_to(new_allocation);
            // begin_ = (--begin()).base();
            begin_ = new_start + (new_cap - 1);
        } else {
            alloc_range_construct_n(--begin(), 1, std::forward<Args>(args)...);
            begin_ = (--begin()).base();
        }

        return front();
    }

    constexpr auto pop_front() noexcept -> void {
        CIEL_PRECONDITION(!empty());

        alloc_range_destroy(begin(), ++begin());
        begin_ = (++begin()).base();
    }

    constexpr auto resize(const size_type count) -> void {
        if (size() >= count) {
            end_ = alloc_range_destroy(begin() + count, end()).base();
            return;
        }

        if (count >= real_capacity()) {
            reserve(count);
        }
        end_ = alloc_range_construct_n(end(), count - size()).base();
    }

    constexpr auto resize(const size_type count, const value_type& value) -> void {
        if (size() >= count) {
            end_ = alloc_range_destroy(begin() + count, end()).base();
            return;
        }

        if (count >= real_capacity()) {
            reserve(count);
        }
        end_ = alloc_range_construct_n(end(), count - size(), value).base();
    }

    constexpr auto swap(circular_buffer& other) noexcept(alloc_traits::propagate_on_container_swap::value ||
                                                         alloc_traits::is_always_equal::value) -> void {
        using std::swap;
        swap(begin_cap_, other.begin_cap_);
        swap(begin_, other.begin_);
        swap(end_, other.end_);
        swap(end_cap_, other.end_cap_);
        swap(allocator_, other.allocator_);
    }

};    // class circular_buffer

template<class T, class Alloc>
[[nodiscard]] constexpr auto operator==(const circular_buffer<T, Alloc>& lhs,
                                        const circular_buffer<T, Alloc>& rhs) -> bool {
    if (lhs.size() != rhs.size()) {
        return false;
    }
    return equal(lhs.begin(), lhs.end(), rhs.begin());
}

// So that we can test more efficiently
template<class T, class Alloc>
[[nodiscard]] constexpr auto operator==(const circular_buffer<T, Alloc>& lhs, std::initializer_list<T> rhs) -> bool {
    if (lhs.size() != rhs.size()) {
        return false;
    }
    return ciel::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<class Iter, class Alloc = allocator<typename iterator_traits<Iter>::value_type>>
circular_buffer(Iter, Iter, Alloc = Alloc()) -> circular_buffer<typename iterator_traits<Iter>::value_type, Alloc>;

NAMESPACE_CIEL_END

namespace std {

template<class T, class Alloc>
constexpr auto swap(ciel::circular_buffer<T, Alloc>& lhs,
                    ciel::circular_buffer<T, Alloc>& rhs) noexcept(noexcept(lhs.swap(rhs))) -> void {
    lhs.swap(rhs);
}

}   // namespace std

#endif // CIELUTILS_INCLUDE_CIEL_CIRCULAR_BUFFER_HPP_