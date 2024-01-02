#ifndef CIELUTILS_INCLUDE_CIEL_SPLIT_BUFFER_HPP_
#define CIELUTILS_INCLUDE_CIEL_SPLIT_BUFFER_HPP_

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

// This class is used in std::deque implementation and it's like a double-ended vector.
// We complete its functionality so that it can be used as a normal container.
// When pushing elements and there is no space this side, we try to shift to other side if there is plenty of space,
// or just expand.
// When it comes to expansion, we try to move old elements to the middle of new space
// and leave some free space at both sides.
// Self assignments like v.emplace_back(v[0]) is not supported so that we can simplify the code.

template<class T, class Allocator = allocator<T>>
class split_buffer {

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

    pointer begin_cap_;
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

    constexpr auto front_spare() const noexcept -> size_type {
        const difference_type res = distance(begin_cap_, begin_);

        CIEL_PRECONDITION(res >= 0);

        return res;
    }

    constexpr auto back_spare() const noexcept -> size_type {
        const difference_type res = distance(end_, end_cap_);

        CIEL_PRECONDITION(res >= 0);

        return res;
    }

    // Note that this will invalidate iterators
    constexpr auto left_shift_n(const size_type n) noexcept -> void {
        CIEL_PRECONDITION(front_spare() >= n);

        const size_type old_size = size();

        pointer new_begin = begin_ - n;
        pointer new_end = new_begin;

        if (old_size >= n) {    // n placement new, size - n move assign, n destroy

            //          ----------
            //
            //  ----------
            //  |      | |       |
            // placement new
            //      move assign
            //            destroy

            size_type i = 0;
            for (; i < n; ++i) {
                alloc_traits::construct(allocator_, new_end, std::move(*(begin_ + i)));
                ++new_end;
            }

            for (; i < old_size; ++i) {
                *new_end = std::move(*(begin_ + i));
                ++new_end;
            }

            end_ = alloc_range_destroy(new_end, end_);
            begin_ = new_begin;

        } else {    // size placement new, size destroy

            //                ----------
            //
            //  ----------
            //  |        |    |        |
            // placement new
            //                 destroy

            for (size_type i = 0; i < old_size; ++i) {
                alloc_traits::construct(allocator_, new_end, std::move(*(begin_ + i)));
                ++new_end;
            }

            alloc_range_destroy(begin_, end_);
            begin_ = new_begin;
            end_ = new_end;
        }
    }

    // Note that this will invalidate iterators
    constexpr auto right_shift_n(const size_type n) noexcept -> void {
        CIEL_PRECONDITION(back_spare() >= n);

        const size_type old_size = size();

        pointer new_end = end_ + n;;
        pointer new_begin = new_end;

        if (old_size >= n) {    // n placement new, size - n move assign, n destroy

            //  ----------
            //
            //          ----------
            //  |       | |      |
            //         placement new
            //      move assign
            //  destroy

            size_type i = 1;
            for (; i <= n; ++i) {
                alloc_traits::construct(allocator_, --new_begin, std::move(*(end_ - i)));
            }

            for (; i <= old_size; ++i) {
                *(--new_begin) = std::move(*(end_ - i));
            }

            alloc_range_destroy(begin_, new_begin);
            begin_ = new_begin;
            end_ = new_end;

        } else {    // size placement new, size destroy

            //  ----------
            //
            //                ----------
            //  |        |    |        |
            //               placement new
            //   destroy

            for (size_type i = 1; i <= old_size; ++i) {
                alloc_traits::construct(allocator_, --new_begin, std::move(*(end_ - i)));
            }

            alloc_range_destroy(begin_, end_);
            begin_ = new_begin;
            end_ = new_end;
        }
    }

    [[nodiscard]] constexpr auto get_new_allocation(const size_type new_cap) -> allocation_result<pointer, size_type> {
        CIEL_PRECONDITION(new_cap > 0);

        if (new_cap > max_size()) [[unlikely]] {
            THROW(std::length_error("ciel::split_buffer reserving size is beyond max_size"));
        }
        return alloc_traits::allocate_at_least(allocator_, new_cap);
    }

    // move old elements to new_start, destroy old elements, deallocate old memory,
    // renew begin_cap_, begin_, end_, end_cap_
    constexpr auto reserve_to(allocation_result<pointer, size_type> new_allocation, const size_type begin_offset = 0)
        noexcept -> void {
        CIEL_PRECONDITION(new_allocation.count > 0);
        CIEL_PRECONDITION(begin_offset + size() <= new_allocation.count);

        pointer new_start = new_allocation.ptr + begin_offset;

        alloc_range_move(new_start, begin_, end_);

        const size_type old_size = size();

        if (begin_cap_) {
            clear();
            alloc_traits::deallocate(allocator_, begin_cap_, capacity());
        }

        begin_cap_ = new_allocation.ptr;
        begin_ = new_start;
        end_ = begin_ + old_size;
        end_cap_ = begin_cap_ + new_allocation.count;
    }

    template<class... Args>
    constexpr auto insert_n(iterator pos, const size_type count, Args&& ... args) -> iterator {
        if (count == 0) [[unlikely]] {
            return pos;
        }

        if (front_spare() >= count &&
                (distance(begin(), pos) < distance(pos, end()) || back_spare() < count)) {
            // move left half to left

            alloc_range_construct_n(begin_ - count, count, std::forward<Args>(args)...);

            iterator old_begin = begin();
            begin_ -= count;

            rotate(begin(), old_begin, pos);
            return pos -= count;
        }

        if (back_spare() < count && front_spare() + back_spare() >= count) {
            const auto diff = count - back_spare();
            left_shift_n(diff);
            pos -= diff;
        }

        if (back_spare() >= count) {    // move right half to right
            iterator old_end = end();
            end_ = alloc_range_construct_n(end_, count, std::forward<Args>(args)...);
            rotate(pos, old_end, end());
            return pos;
        }

        // When it comes to expansion, we need to construct new elements directly on new space,
        // if it throws then has no effect. And move construct two ranges divided by pos on new space

        const size_type new_size = size() + count;
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

        if (begin_cap_) {
            alloc_range_move(new_start, begin_, to_address(pos));
            alloc_range_move(new_pos + count, to_address(pos), end_);

            clear();
            alloc_traits::deallocate(allocator_, begin_cap_, capacity());
        }

        begin_cap_ = new_start;
        begin_ = new_start;
        end_ = begin_ + new_size;
        end_cap_ = begin_cap_ + new_cap;

        return iterator(new_pos);
    }

    constexpr auto clear_and_get_cap_no_less_than(const size_type cap, const size_type begin_offset = 0) -> void {
        CIEL_PRECONDITION(cap >= begin_offset);

        clear();
        begin_ = begin_cap_;
        end_ = begin_;

        if (capacity() < cap) {
            if (begin_cap_) {
                alloc_traits::deallocate(allocator_, begin_cap_, capacity());

                // Prevent exceptions throwing and double free in destructor when asking for memory
                begin_cap_ = nullptr;
                end_cap_ = nullptr;
                begin_ = nullptr;
                end_ = nullptr;
            }
            begin_cap_ = alloc_traits::allocate(allocator_, cap);
            end_cap_ = begin_cap_ + cap;
            begin_ = begin_cap_ + begin_offset;
            end_ = begin_;
        }
    }

public:
    constexpr split_buffer() noexcept(noexcept(allocator_type()))
        : begin_cap_(nullptr), begin_(nullptr), end_(nullptr), end_cap_(nullptr), allocator_() {}

    constexpr explicit split_buffer(const allocator_type& alloc) noexcept
        : begin_cap_(nullptr), begin_(nullptr), end_(nullptr), end_cap_(nullptr), allocator_(alloc) {}

    constexpr split_buffer(const size_type count, const value_type& value,
                           const allocator_type& alloc = allocator_type())
        : split_buffer(alloc) {
        if (count > 0) [[likely]] {
            begin_cap_ = alloc_traits::allocate(allocator_, count);
            end_cap_ = begin_cap_ + count;

            begin_ = begin_cap_;

            CIEL_TRY {
                end_ = alloc_range_construct_n(begin_, count, value);
            } CIEL_CATCH (...) {
                alloc_traits::deallocate(allocator_, begin_cap_, capacity());
                CIEL_THROW;
            }
        }
    }

    constexpr explicit split_buffer(const size_type count, const allocator_type& alloc = allocator_type())
        : split_buffer(alloc) {
        if (count > 0) [[likely]] {
            begin_cap_ = alloc_traits::allocate(allocator_, count);
            end_cap_ = begin_cap_ + count;

            begin_ = begin_cap_;

            CIEL_TRY {
                end_ = alloc_range_construct_n(begin_, count);
            } CIEL_CATCH (...) {
                alloc_traits::deallocate(allocator_, begin_cap_, capacity());
                CIEL_THROW;
            }
        }
    }

    template<class Iter>
        requires is_exactly_input_iterator<Iter>::value
    constexpr split_buffer(Iter first, Iter last, const allocator_type& alloc = allocator_type())
        : split_buffer(alloc) {
        CIEL_TRY {
            while (first != last) {
                emplace_back(*first);
                ++first;
            }
        } CIEL_CATCH (...) {
            clear();
            alloc_traits::deallocate(allocator_, begin_cap_, capacity());
            CIEL_THROW;
        }
    }

    template<class Iter>
        requires is_forward_iterator<Iter>::value
    constexpr split_buffer(Iter first, Iter last, const allocator_type& alloc = allocator_type())
        : split_buffer(alloc) {
        if (auto count = ciel::distance(first, last); count > 0) [[likely]] {
            begin_cap_ = alloc_traits::allocate(allocator_, count);
            end_cap_ = begin_cap_ + count;

            begin_ = begin_cap_;

            CIEL_TRY {
                end_ = alloc_range_construct(begin_, first, last);
            } CIEL_CATCH (...) {
                alloc_traits::deallocate(allocator_, begin_cap_, capacity());
                CIEL_THROW;
            }
        }
    }

    constexpr split_buffer(const split_buffer& other)
        : split_buffer(other.begin(), other.end(),
                 alloc_traits::select_on_container_copy_construction(other.get_allocator())) {}

    constexpr split_buffer(const split_buffer& other, const allocator_type& alloc)
        : split_buffer(other.begin(), other.end(), alloc) {}

    constexpr split_buffer(split_buffer&& other) noexcept
        : begin_cap_(other.begin_cap_), begin_(other.begin_), end_(other.end_), end_cap_(other.end_cap_),
          allocator_(std::move(other.allocator_)) {
        other.begin_cap_ = nullptr;
        other.begin_ = nullptr;
        other.end_ = nullptr;
        other.end_cap_ = nullptr;
    }

    constexpr split_buffer(split_buffer&& other, const allocator_type& alloc) {
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
            split_buffer(other, alloc).swap(*this);
        }
    }

    constexpr split_buffer(std::initializer_list<value_type> init, const allocator_type& alloc = allocator_type())
        : split_buffer(init.begin(), init.end(), alloc) {}

    constexpr ~split_buffer() {
        if (begin_cap_) {
            clear();
            alloc_traits::deallocate(allocator_, begin_cap_, capacity());
        }
    }

    constexpr auto operator=(const split_buffer& other) -> split_buffer& {
        if (this == addressof(other)) [[unlikely]] {
            return *this;
        }

        if (alloc_traits::propagate_on_container_copy_assignment::value) {
            if (allocator_ != other.allocator_) {
                split_buffer(other.allocator_).swap(*this);
                assign(other.begin(), other.end());
                return *this;
            }

            allocator_ = other.allocator_;
        }
        assign(other.begin(), other.end());
        return *this;
    }

    constexpr auto operator=(split_buffer&& other)
        noexcept(alloc_traits::propagate_on_container_move_assignment::value ||
                 alloc_traits::is_always_equal::value) -> split_buffer& {
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
            alloc_traits::deallocate(allocator_, begin_cap_, capacity());
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

    constexpr auto operator=(std::initializer_list<value_type> ilist) -> split_buffer& {
        assign(ilist.begin(), ilist.end());
        return *this;
    }

    constexpr auto assign(const size_type count, const value_type& value) -> void {
        if (back_spare() + size() < count) {
            if (const size_type diff = count - back_spare() - size(); front_spare() >= diff) {
                left_shift_n(diff);
            } else {
                clear_and_get_cap_no_less_than(count);
                end_ = alloc_range_construct_n(begin_, count, value);
                return;
            }

        } else if (size() > count) {
            end_ = alloc_range_destroy(begin_ + count, end_);;
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

        if (back_spare() + size() < count) {
            if (const size_type diff = count - back_spare() - size(); front_spare() >= diff) {
                left_shift_n(diff);
            } else {
                clear_and_get_cap_no_less_than(count);
                end_ = alloc_range_construct(begin_, first, last);
                return;
            }

        } else if (size() > count) {
            end_ = alloc_range_destroy(begin_ + count, end_);;
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
            THROW(std::out_of_range("pos is not within the range of ciel::split_buffer"));
        }
        return begin_[pos];
    }

    [[nodiscard]] constexpr auto at(const size_type pos) const -> const_reference {
        if (pos >= size()) [[unlikely]] {
            THROW(std::out_of_range("pos is not within the range of ciel::split_buffer"));
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

    [[deprecated("This data is not the begin of the allocated memory")]]
    [[nodiscard]] constexpr auto data() noexcept -> T* {
        return begin_;
    }

    [[deprecated("This data is not the begin of the allocated memory")]]
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

    constexpr auto reserve_front_spare(const size_type new_spare, const size_type expected_cap = 0) -> void {
        if (new_spare <= front_spare()) {
            return;
        }

        if (new_spare <= front_spare() + back_spare()) {
            right_shift_n(new_spare - front_spare());

            CIEL_POSTCONDITION(new_spare <= front_spare());
            return;
        }

        auto new_allocation = get_new_allocation(max(new_spare + size(), expected_cap));
        reserve_to(new_allocation, new_spare);

        CIEL_POSTCONDITION(new_spare <= front_spare());
    }

    constexpr auto reserve_back_spare(const size_type new_spare, const size_type expected_cap = 0) -> void {
        if (new_spare <= back_spare()) {
            return;
        }

        if (new_spare <= front_spare() + back_spare()) {
            left_shift_n(new_spare - back_spare());

            CIEL_POSTCONDITION(new_spare <= back_spare());
            return;
        }

        auto new_allocation = get_new_allocation(max(new_spare + size(), expected_cap));
        reserve_to(new_allocation);

        CIEL_POSTCONDITION(new_spare <= back_spare());
    }

    [[nodiscard]] constexpr auto capacity() const noexcept -> size_type {
        return end_cap_ - begin_cap_;
    }

    constexpr auto shrink_to_fit() -> void {
        if (front_spare() == 0 && back_spare() == 0) [[unlikely]] {
            return;
        }

        if (size() > 0) {
            pointer new_start = alloc_traits::allocate(allocator_, size());
            reserve_to({new_start, size()});

        } else if (begin_cap_) {
            alloc_traits::deallocate(allocator_, begin_cap_, capacity());
            begin_cap_ = nullptr;
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

        if (front_spare() >= static_cast<size_type>(count) &&
                (distance(begin(), pos) < distance(pos, end()) ||
                back_spare() < static_cast<size_type>(count))) {
            // move left half to left

            alloc_range_construct(begin_ - count, first, last);

            iterator old_begin = begin();
            begin_ -= count;

            rotate(begin(), old_begin, pos);
            return pos -= count;
        }

        if (back_spare() < static_cast<size_type>(count) &&
                front_spare() + back_spare() >= static_cast<size_type>(count)) {
            const auto diff = count - back_spare();
            left_shift_n(diff);
            pos -= diff;
        }

        if (back_spare() >= static_cast<size_type>(count)) {    // move right half to right
            iterator old_end = end();
            end_ = alloc_range_construct(end_, first, last);
            rotate(pos, old_end, end());
            return pos;
        }

        // When it comes to expansion, we need to construct new elements directly on new space,
        // if it throws then has no effect. And move construct two ranges divided by pos on new space

        const size_type new_size = size() + count;
        size_type new_cap = capacity() ? capacity() * 2 : 1;
        while (new_size > new_cap) {
            new_cap *= 2;
        }

        const size_type idx = pos - begin();
        pointer new_start = alloc_traits::allocate(allocator_, new_cap);
        pointer new_pos = new_start + idx;

        CIEL_TRY {
            alloc_range_construct(new_pos, first, last);
        } CIEL_CATCH (...) {
            alloc_traits::deallocate(allocator_, new_start, new_cap);
            CIEL_THROW;
        }

        if (begin_cap_) {
            alloc_range_move(new_start, begin_, to_address(pos));
            alloc_range_move(new_pos + count, to_address(pos), end_);

            clear();
            alloc_traits::deallocate(allocator_, begin_cap_, capacity());
        }

        begin_cap_ = new_start;
        begin_ = new_start;
        end_ = begin_ + new_size;
        end_cap_ = begin_cap_ + new_cap;

        return iterator(new_pos);
    }

    constexpr auto insert(iterator pos, std::initializer_list<value_type> ilist) -> iterator {
        return insert(pos, ilist.begin(), ilist.end());
    }

    template<class... Args>
    constexpr auto emplace(iterator pos, Args&& ... args) -> iterator {
        if (pos == end()) {
            emplace_back(std::forward<Args>(args)...);
            return iterator(end_ - 1);    // There is possiblity of expansion, don't return iterator(pos)
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
            pointer old_begin = begin_;

            begin_ = move_backward(begin(), first, last).base();

            alloc_range_destroy(old_begin, begin_);

        } else {
            pointer new_end = move(last, end(), first).base();

            end_ = alloc_range_destroy(new_end, end_);
        }

        return begin() + begin_first_distance;
    }

    constexpr auto push_back(const value_type& value) -> void {
        emplace_back(value);
    }

    constexpr auto push_back(value_type&& value) -> void {
        emplace_back(std::move(value));
    }

    // Comparing with vector growing factor: get n * 2 memory, move n elements and get n new space,
    // it's terrible if we shift one (move n elements) to get 1 vacant space for emplace,
    // so only if there is plenty of space at other side will we consider shifting.
    // This situation may be seen when it's used as queue's base container.
    template<class... Args>
    constexpr auto emplace_back(Args&& ... args) -> reference {
        if (back_spare() == 0) {

            if (front_spare() > size()) [[unlikely]] {   // move size elements to get more than size / 2 vacant space
                left_shift_n(max<size_type>(front_spare() / 2, 1));
                end_ = alloc_range_construct_n(end_, 1, std::forward<Args>(args)...);

            } else {
                auto new_allocation = get_new_allocation(capacity() ? capacity() * 2 : 1);
                pointer new_start = new_allocation.ptr;
                const size_type new_cap = new_allocation.count;

                CIEL_TRY {
                    alloc_range_construct_n(new_start + front_spare() + size(), 1, std::forward<Args>(args)...);
                } CIEL_CATCH (...) {
                    alloc_traits::deallocate(allocator_, new_start, new_cap);
                    CIEL_THROW;
                }
                reserve_to(new_allocation, front_spare());
                ++end_;
            }
        } else {
            end_ = alloc_range_construct_n(end_, 1, std::forward<Args>(args)...);
        }

        return back();
    }

    constexpr auto pop_back() noexcept -> void {
        CIEL_PRECONDITION(!empty());

        end_ = alloc_range_destroy(end_ - 1, end_);
    }

    constexpr auto push_front(const value_type& value) -> void {
        emplace_front(value);
    }

    constexpr auto push_front(value_type&& value) -> void {
        emplace_front(std::move(value));
    }

    template<class... Args>
    constexpr auto emplace_front(Args&& ... args) -> reference {
        if (front_spare() == 0) {

            if (back_spare() > size()) [[unlikely]] {   // move size elements to get more than size / 2 vacant space
                right_shift_n(max<size_type>(back_spare() / 2, 1));
                alloc_range_construct_n(begin_ - 1, 1, std::forward<Args>(args)...);
                --begin_;

            } else {
                auto new_allocation = get_new_allocation(capacity() ? capacity() * 2 : 1);
                pointer new_start = new_allocation.ptr;
                const size_type new_cap = new_allocation.count;

                CIEL_TRY {
                    alloc_range_construct_n(new_start + (new_cap - size() - back_spare() - 1), 1,
                                            std::forward<Args>(args)...);
                } CIEL_CATCH (...) {
                    alloc_traits::deallocate(allocator_, new_start, new_cap);
                    CIEL_THROW;
                }
                reserve_to(new_allocation, new_cap - size() - back_spare());
                --begin_;
            }
        } else {
            alloc_range_construct_n(begin_ - 1, 1, std::forward<Args>(args)...);
            --begin_;
        }

        return front();
    }

    constexpr auto pop_front() noexcept -> void {
        CIEL_PRECONDITION(!empty());

        alloc_range_destroy(begin_, begin_ + 1);
        ++begin_;
    }

    constexpr auto resize(const size_type count) -> void {
        if (size() >= count) {
            end_ = alloc_range_destroy(begin_ + count, end_);
            return;
        }

        reserve_back_spare(count - size());
        end_ = alloc_range_construct_n(end_, count - size());
    }

    constexpr auto resize(const size_type count, const value_type& value) -> void {
        if (size() >= count) {
            end_ = alloc_range_destroy(begin_ + count, end_);
            return;
        }

        reserve_back_spare(count - size());
        end_ = alloc_range_construct_n(end_, count - size(), value);
    }

    constexpr auto swap(split_buffer& other) noexcept(alloc_traits::propagate_on_container_swap::value ||
                                                      alloc_traits::is_always_equal::value) -> void {
        using std::swap;
        swap(begin_cap_, other.begin_cap_);
        swap(begin_, other.begin_);
        swap(end_, other.end_);
        swap(end_cap_, other.end_cap_);
        swap(allocator_, other.allocator_);
    }

};  // class split_buffer

template<class T, class Alloc>
[[nodiscard]] constexpr auto operator==(const split_buffer<T, Alloc>& lhs, const split_buffer<T, Alloc>& rhs) -> bool {
    if (lhs.size() != rhs.size()) {
        return false;
    }
    return equal(lhs.begin(), lhs.end(), rhs.begin());
}

// So that we can test more efficiently
template<class T, class Alloc>
[[nodiscard]] constexpr auto operator==(const split_buffer<T, Alloc>& lhs, std::initializer_list<T> rhs) -> bool {
    if (lhs.size() != rhs.size()) {
        return false;
    }
    return ciel::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<class Iter, class Alloc = allocator<typename iterator_traits<Iter>::value_type>>
split_buffer(Iter, Iter, Alloc = Alloc()) -> split_buffer<typename iterator_traits<Iter>::value_type, Alloc>;

NAMESPACE_CIEL_END

namespace std {

template<class T, class Alloc>
constexpr auto swap(ciel::split_buffer<T, Alloc>& lhs,
                    ciel::split_buffer<T, Alloc>& rhs) noexcept(noexcept(lhs.swap(rhs))) -> void {
    lhs.swap(rhs);
}

}   // namespace std

#endif // CIELUTILS_INCLUDE_CIEL_SPLIT_BUFFER_HPP_