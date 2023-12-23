#ifndef CIELUTILS_INCLUDE_CIEL_VECTOR_HPP_
#define CIELUTILS_INCLUDE_CIEL_VECTOR_HPP_

#include <ciel/algorithm_impl/copy_n.hpp>
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

// TODO: operator<=>

template<class T, class Allocator = allocator<T>>
class vector {

    static_assert(is_same_v<typename Allocator::value_type, T>);

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
                alloc_traits::construct(allocator_, end++, std::forward<Args>(args)...);
            }
            return end;
        } CIEL_CATCH(...) {
            alloc_range_destroy(begin, --end);
            CIEL_THROW;
        }
    }

    template<class Iter>
        requires is_forward_iterator<Iter>::value
    constexpr auto alloc_range_construct(pointer begin, Iter first, Iter last) -> pointer {
        pointer end = begin;
        CIEL_TRY {
            while (first != last) {
                alloc_traits::construct(allocator_, end++, *first++);
            }
            return end;
        } CIEL_CATCH(...) {
            alloc_range_destroy(begin, --end);
            CIEL_THROW;
        }
    }

    constexpr auto alloc_range_move(pointer begin, pointer first, pointer last) noexcept -> pointer {
        pointer end = begin;
        while (first != last) {
            alloc_traits::construct(allocator_, end++, std::move(*first++));
        }
        return end;
    }

    // Used in insert and emplace
    //
    // O  O  O  O  O  O  O  O  O  O
    //
    // insert three elements
    //          -  -  -
    // O  O  O  N  N  N  O  O  O  O  O  O  O
    //                   ----------  -------
    //                   move assign placement new
    constexpr auto alloc_range_nothrow_move_backward(pointer end, pointer first, pointer last) noexcept -> pointer {
        pointer begin = end;
        pointer boundary = last;
        while (first != last && begin != boundary) {
            alloc_traits::construct(allocator_, --begin, std::move(*--last));
        }
        while (first != last) {
            *--begin = std::move(*--last);
        }
        return end;
    }

    // step1: only allocate new memory
    [[nodiscard]] constexpr auto reserve_step1(const size_type new_cap) -> pointer {
        if (new_cap > max_size()) {
            THROW(std::length_error("ciel::vector reserving size is beyond max_size"));
        }
        return alloc_traits::allocate(allocator_, new_cap);
    }

    // step2: move old elements, destroy old elements, deallocate old memory
    constexpr auto reserve_step2(pointer new_start, const size_type new_cap)
        noexcept(is_nothrow_move_constructible_v<value_type>) -> void {
        if constexpr (is_nothrow_move_constructible_v<value_type>) {    // strong exception safety
            alloc_range_move(new_start, begin_, end_);
        } else {
            CIEL_TRY {
                alloc_range_construct(new_start, begin_, end_);
            } CIEL_CATCH(...) {
                alloc_traits::deallocate(allocator_, new_start, new_cap);
                CIEL_THROW;
            }
        }
        const size_type old_size = size();
        clear();
        alloc_traits::deallocate(allocator_, begin_, capacity());
        begin_ = new_start;
        end_ = begin_ + old_size;
        end_cap_ = begin_ + new_cap;
    }

    // O  O  O  O
    //          -  -
    // insert two elements
    //
    // O  O  O  N  N  O
    //          -  -
    //          *  *  *
    //          move assign
    //             placement new
    //                move construct
    template<class Arg>
    constexpr auto insert_n(iterator pos, const size_type count, Arg&& arg) -> iterator {
        if (!count) {
            return pos;
        }
        // When it comes to expansion, we need to move construct two ranges divided by pos on new space,
        // and construct new elements directly on that
        if (const size_type new_size = size() + count; new_size > capacity()) {
            size_type new_cap = capacity() ? capacity() * 2 : 1;
            while (new_size > new_cap) {
                new_cap *= 2;
            }

            size_type idx = pos - begin();
            pointer new_start = alloc_traits::allocate(allocator_, new_cap);
            pointer new_pos = new_start + idx;

            alloc_range_move(new_start, begin_, pos.base());
            alloc_range_move(new_pos + count, pos.base(), end_);

            clear();
            alloc_traits::deallocate(allocator_, begin_, capacity());
            begin_ = new_start;
            end_ = begin_ + new_size;
            end_cap_ = begin_ + new_cap;
            return iterator(alloc_range_construct_n(new_pos, count, std::forward<Arg>(arg)) - 1);
        } else {
            pointer boundary = end_;
            end_ = alloc_range_nothrow_move_backward(end_ + count, pos.base(), end_);
            const size_type left_movement = boundary - pos.base();
            const size_type loop_break = left_movement < count ? left_movement : count;
            for (size_type i = 0; i < loop_break; ++i) {    // assignments
                *(pos++) = std::forward<Arg>(arg);
            }
            if (left_movement < count) {    // After assignments, there are elements need to be constructed
                pos = iterator(alloc_range_construct_n(boundary, count - left_movement, std::forward<Arg>(arg)) - 1);
            }
            return pos;
        }
    }

    constexpr auto clear_and_get_cap_no_less_than(const size_type size) -> void {
        clear();
        if (capacity() < size) {
            alloc_traits::deallocate(allocator_, begin_, capacity());
            begin_ = nullptr;    // Prevent exceptions throwing and double free in destructor when asking for memory
            begin_ = alloc_traits::allocate(allocator_, size);
            end_cap_ = begin_ + size;
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
        if (count > 0) {
            begin_ = alloc_traits::allocate(allocator_, count);
            end_cap_ = begin_ + count;
            CIEL_TRY {
                end_ = alloc_range_construct_n(begin_, count, value);
            } CIEL_CATCH(...) {
                alloc_traits::deallocate(allocator_, begin_, capacity());
                CIEL_THROW;
            }
        }
    }

    constexpr explicit vector(const size_type count, const allocator_type& alloc = allocator_type())
        : vector(alloc) {
        if (count > 0) {
            begin_ = alloc_traits::allocate(allocator_, count);
            end_cap_ = begin_ + count;
            CIEL_TRY {
                end_ = alloc_range_construct_n(begin_, count);
            } CIEL_CATCH(...) {
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
            } CIEL_CATCH(...) {
            clear();
            alloc_traits::deallocate(allocator_, begin_, capacity());
            CIEL_THROW;
        }
    }

    template<class Iter>
        requires is_forward_iterator<Iter>::value
    constexpr vector(Iter first, Iter last, const allocator_type& alloc = allocator_type())
        : vector(alloc) {
        if (auto count = ciel::distance(first, last); count > 0) {
            begin_ = alloc_traits::allocate(allocator_, count);
            end_cap_ = begin_ + count;
            CIEL_TRY {
                end_ = alloc_range_construct(begin_, first, last);
            } CIEL_CATCH(...) {
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
        if (this == addressof(other)) {
            return *this;
        }
        if (alloc_traits::propagate_on_container_copy_assignment::value) {
            if (allocator_ != other.allocator_) {
                vector(other.allocator_).swap(*this);
                clear_and_get_cap_no_less_than(other.size());
                end_ = alloc_range_construct(begin_, other.begin(), other.end());
                return *this;
            }
            allocator_ = other.allocator_;
        }
        clear_and_get_cap_no_less_than(other.size());
        end_ = alloc_range_construct(begin_, other.begin(), other.end());
        return *this;
    }

    constexpr auto operator=(vector&& other)
        noexcept(alloc_traits::propagate_on_container_move_assignment::value ||
                 alloc_traits::is_always_equal::value) -> vector& {
        if (this == addressof(other)) {
            return *this;
        }
        if (!alloc_traits::propagate_on_container_move_assignment::value && allocator_ != other.allocator_) {
            clear_and_get_cap_no_less_than(other.size());
            end_ = alloc_range_construct(begin_, other.begin(), other.end());
            return *this;
        }
        if (alloc_traits::propagate_on_container_move_assignment::value) {
            allocator_ = other.allocator_;
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
        clear_and_get_cap_no_less_than(ilist.size());
        end_ = alloc_range_construct(begin_, ilist.begin(), ilist.end());
        return *this;
    }

    constexpr auto assign(const size_type count, const value_type& value) -> void {
        clear_and_get_cap_no_less_than(count);
        end_ = alloc_range_construct_n(begin_, count, value);
    }

    template<class Iter>
        requires is_forward_iterator<Iter>::value
    constexpr auto assign(Iter first, Iter last) -> void {
        clear_and_get_cap_no_less_than(ciel::distance(first, last));
        end_ = alloc_range_construct(begin_, first, last);
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
        clear_and_get_cap_no_less_than(ilist.size());
        end_ = alloc_range_construct(begin_, ilist.begin(), ilist.end());
    }

    [[nodiscard]] constexpr auto get_allocator() const noexcept -> allocator_type {
        return allocator_;
    }

    [[nodiscard]] constexpr auto at(const size_type pos) -> reference {
        if (pos >= size()) {
            THROW(std::out_of_range("pos is not within the range of ciel::vector"));
        }
        return begin_[pos];
    }

    [[nodiscard]] constexpr auto at(const size_type pos) const -> const_reference {
        if (pos >= size()) {
            THROW(std::out_of_range("pos is not within the range of ciel::vector"));
        }
        return begin_[pos];
    }

    [[nodiscard]] constexpr auto operator[](const size_type pos) -> reference {
        return begin_[pos];
    }

    [[nodiscard]] constexpr auto operator[](const size_type pos) const -> const_reference {
        return begin_[pos];
    }

    [[nodiscard]] constexpr auto front() -> reference {
        return begin_[0];
    }

    [[nodiscard]] constexpr auto front() const -> const_reference {
        return begin_[0];
    }

    [[nodiscard]] constexpr auto back() -> reference {
        return *(end_ - 1);
    }

    [[nodiscard]] constexpr auto back() const -> const_reference {
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
        return begin() == end();
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
    constexpr auto reserve(const size_type new_cap) ->void {
        if (new_cap <= capacity()) {
            return;
        }
        pointer new_start = reserve_step1(new_cap);
        reserve_step2(new_start, new_cap);
    }

    [[nodiscard]] constexpr auto capacity() const noexcept -> size_type {
        return end_cap_ - begin_;
    }

    constexpr auto shrink_to_fit() -> void {
        if (size() == capacity()) {
            return;
        }
        pointer new_start = alloc_traits::allocate(allocator_, size());
        reserve_step2(new_start, size());
    }

    constexpr auto clear() noexcept -> void {
        end_ = alloc_range_destroy(begin_, end_);
    }

    constexpr auto insert(const_iterator pos, const value_type& value) -> iterator {
        return insert_n(pos, 1, value);
    }

    constexpr auto insert(const_iterator pos, value_type&& value) -> iterator {
        return insert_n(pos, 1, std::move(value));
    }

    constexpr auto insert(const_iterator pos, const size_type count, const value_type& value) -> iterator {
        return insert_n(pos, count, value);
    }

    // We construct all at the end at first, then rotate them to the right place
    template<class Iter>
        requires is_exactly_input_iterator<Iter>::value
    constexpr auto insert(const_iterator pos, Iter first, Iter last) -> iterator {
        auto pos_index = pos - begin();
        size_type old_size = size();
        while (first != last) {
            emplace_back(*first);
            ++first;
        }
        return rotate(begin() + pos_index, begin() + old_size, end()) - 1;
    }

    template<class Iter>
        requires is_forward_iterator<Iter>::value
    constexpr auto insert(const_iterator pos, Iter first, Iter last) -> iterator {
        auto count = ciel::distance(first, last);
        if (count <= 0) {
            return pos;
        }
        // When it comes to expansion, we need to move construct two ranges divided by pos on new space,
        // and construct new elements directly on that
        if (const size_type new_size = size() + count; new_size > capacity()) {
            size_type new_cap = capacity() ? capacity() * 2 : 1;
            while (new_size > new_cap) {
                new_cap *= 2;
            }

            size_type idx = pos - begin();
            pointer new_start = alloc_traits::allocate(allocator_, new_cap);
            pointer new_pos = new_start + idx;

            alloc_range_move(new_start, begin_, const_cast<pointer>(pos.base()));
            alloc_range_move(new_pos + count, const_cast<pointer>(pos.base()), end_);

            clear();
            alloc_traits::deallocate(allocator_, begin_, capacity());
            begin_ = new_start;
            end_ = begin_ + new_size;
            end_cap_ = begin_ + new_cap;
            return iterator(alloc_range_construct(new_pos, first, last) - 1);
        } else {    // No expansion, we need to take care of initialized and uninitialized memory separately
            pointer boundary = end_;
            end_ = alloc_range_nothrow_move_backward(end_ + count, const_cast<pointer>(pos.base()), end_);
            auto left_movement = boundary - pos.base();
            auto loop_break = left_movement < count ? left_movement : count;
            ciel::copy_n(first, loop_break, iterator(pos));
            if (left_movement < count) {    // After assignments, there are elements need to be constructed
                pos = iterator(alloc_range_construct(boundary, first + left_movement, last) - 1);
            }
            return pos;
        }
    }

    constexpr auto insert(const_iterator pos, std::initializer_list<value_type> ilist) -> iterator {
        return insert(pos, ilist.begin(), ilist.end());
    }

    template<class... Args>
    constexpr auto emplace(const_iterator pos, Args&& ... args) -> iterator {
        if (pos == end()) {
            emplace_back(std::forward<Args>(args)...);
            return iterator(end_ - 1);    // There is possiblity of expansion, don't return iterator(pos)
        }
        return insert_n(pos, 1, value_type(std::forward<Args>(args)...));
    }

    constexpr auto erase(const_iterator pos) -> iterator {
        return erase(pos, pos + 1);
    }

    constexpr auto erase(const_iterator first, const_iterator last) -> iterator {
        auto distance = ciel::distance(first, last);
        if (!distance) {
            return last;
        }
        iterator new_end = move(iterator(last), end(), iterator(first));
        end_ = alloc_range_destroy(new_end.base(), end_);
        return end();
    }

    constexpr auto push_back(const T& value) -> void {
        emplace_back(value);
    }

    constexpr auto push_back(T&& value) -> void {
        emplace_back(std::move(value));
    }

    template<class... Args>
    constexpr auto emplace_back(Args&& ... args) -> reference {
        if (size_type new_cap = capacity(); size() == new_cap) {
            new_cap = new_cap ? 2 * new_cap : 1;
            pointer new_start = reserve_step1(new_cap);
            CIEL_TRY {
                alloc_range_construct_n(new_start + size(), 1, std::forward<Args>(args)...);
            } CIEL_CATCH(...) {
                alloc_traits::deallocate(allocator_, new_start, new_cap);
                CIEL_THROW;
            }
            reserve_step2(new_start, new_cap);
            ++end_;
        } else {
            end_ = alloc_range_construct_n(end_, 1, std::forward<Args>(args)...);
        }
        return back();
    }

    constexpr auto pop_back() noexcept -> void {
        if (!empty()) {
            end_ = alloc_range_destroy(end_ - 1, end_);
        }
    }

    constexpr auto resize(const size_type count) -> void {
        if (size() >= count) {
            end_ = alloc_range_destroy(end_ - (size() - count), end_);
        } else if (count > capacity()) {
            reserve(count);
            end_ = alloc_range_construct_n(end_, count - size());
        } else {
            end_ = alloc_range_construct_n(end_, count - size());
        }
    }

    constexpr auto resize(const size_type count, const value_type& value) -> void {
        if (size() >= count) {
            end_ = alloc_range_destroy(end_ - (size() - count), end_);
        } else if (count > capacity()) {
            reserve(count);
            end_ = alloc_range_construct_n(end_, count - size(), value);
        } else {
            end_ = alloc_range_construct_n(end_, count - size(), value);
        }
    }

    constexpr auto swap(vector& other)
        noexcept(alloc_traits::propagate_on_container_swap::value || alloc_traits::is_always_equal::value) -> void {
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

template<legacy_input_iterator Iter, class Alloc = allocator<typename iterator_traits<Iter>::value_type>>
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