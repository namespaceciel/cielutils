#ifndef CIELUTILS_INCLUDE_CIEL_DEQUE_HPP_
#define CIELUTILS_INCLUDE_CIEL_DEQUE_HPP_

#include <ciel/algorithm_impl/copy_n.hpp>
#include <ciel/algorithm_impl/move.hpp>
#include <ciel/algorithm_impl/move_backward.hpp>
#include <ciel/algorithm_impl/remove.hpp>
#include <ciel/algorithm_impl/remove_if.hpp>
#include <ciel/algorithm_impl/rotate.hpp>
#include <ciel/config.hpp>
#include <ciel/list.hpp>
#include <ciel/memory_impl/unique_ptr.hpp>
#include <cstddef>
#include <stdexcept>

NAMESPACE_CIEL_BEGIN

// TODO: operator<=>

// To be simple, the "map" of deque (control block of every subarray) is list-implemented,
// while the common implemention is vector-like.
// It can avoid reallocations.
//
// Also to be simple, empty deque will still allocate a block of memory,
// which changes the behavior of move construction.

template<class T>
constexpr auto deque_subarray_size() noexcept -> size_t {
    return ((sizeof(T) <= 4) ? 64 : ((sizeof(T) <= 8) ? 32 : ((sizeof(T) <= 16) ? 16 : ((sizeof(T) <= 32) ? 8 : 4))));
}

template<class T, class Pointer, class Reference, class MapIterator, size_t SubarraySize>
class deque_iterator {
public:
    using difference_type   = ptrdiff_t;
    using value_type        = T;
    using pointer           = Pointer;
    using reference         = Reference;
    using iterator_category = random_access_iterator_tag;
    using iterator_concept  = random_access_iterator_tag;

private:
    T* cur_;
    MapIterator node_;

    template<class, class, class, class, size_t>
    friend class deque_iterator;
    template<class, class, size_t>
    friend class deque;
    template<class U, class Pointer1, class Pointer2, class Reference1, class Reference2, class M, size_t S>
    friend auto operator-(const deque_iterator<U, Pointer1, Reference1, M, S>&,
                          const deque_iterator<U, Pointer2, Reference2, M, S>&)
        -> typename iterator_traits<deque_iterator<U, Pointer1, Reference1, M, S>>::difference_type;

public:
    [[nodiscard]] auto start() const noexcept -> T* {
        return (*node_).get();   // FIXME: node_->get() error
    }

    // Only used as sentinel
    [[nodiscard]] auto finish() const noexcept -> T* {
        return start() + SubarraySize;
    }

    deque_iterator() noexcept = default;

    deque_iterator(T* c, const MapIterator& n) noexcept : cur_(c), node_(n) {}

    deque_iterator(const deque_iterator&) noexcept = default;
    deque_iterator(deque_iterator&&) noexcept = default;

    template<class P, class R, class M>
    deque_iterator(const deque_iterator<T, P, R, M, SubarraySize>& other) noexcept
        : cur_(other.base()), node_(other.node_) {}

    ~deque_iterator() = default;

    auto operator=(const deque_iterator&) noexcept -> deque_iterator& = default;
    auto operator=(deque_iterator&&) noexcept -> deque_iterator& = default;

    [[nodiscard]] auto operator->() const noexcept -> pointer {
        return cur_;
    }

    [[nodiscard]] auto operator*() const noexcept -> reference {
        return *cur_;
    }

    auto operator++() noexcept -> deque_iterator& {
        if (++cur_ == finish()) {
            ++node_;
            cur_ = start();
        }
        return *this;
    }

    [[nodiscard]] auto operator++(int) noexcept -> deque_iterator {
        deque_iterator res(*this);
        ++*this;
        return res;
    }

    auto operator--() noexcept -> deque_iterator& {
        if (cur_ == start()) {
            --node_;
            cur_ = finish() - 1;
        } else {
            --cur_;
        }
        return *this;
    }

    [[nodiscard]] auto operator--(int) noexcept -> deque_iterator {
        deque_iterator res(*this);
        --*this;
        return res;
    }

    auto operator+=(const difference_type n) noexcept -> deque_iterator& {
        difference_type offset = n + (cur_ - start());
        if (offset >= 0 && static_cast<size_t>(offset) < SubarraySize) {
            cur_ += n;
        } else {
            advance(node_, offset > 0 ? offset / static_cast<difference_type>(SubarraySize) :
                                        offset / static_cast<difference_type>(SubarraySize) - 1);
            offset %= SubarraySize;
            cur_ = start() + offset;
        }
        return *this;
    }

    auto operator-=(const difference_type n) noexcept -> deque_iterator& {
        return *this += -n;
    }

    [[nodiscard]] auto operator+(const difference_type n) const noexcept -> deque_iterator {
        deque_iterator res(*this);
        return res += n;
    }

    [[nodiscard]] auto operator-(const difference_type n) const noexcept -> deque_iterator {
        deque_iterator res(*this);
        return res -= n;
    }

    [[nodiscard]] auto base() const noexcept -> T* {
        return cur_;
    }

};    // struct deque_iterator

template<class T, class Pointer1, class Pointer2, class Reference1,
    class Reference2, class MapIterator, size_t SubarraySize>
[[nodiscard]] auto operator==(const deque_iterator<T, Pointer1, Reference1, MapIterator, SubarraySize>& lhs,
                              const deque_iterator<T, Pointer2, Reference2, MapIterator, SubarraySize>& rhs) -> bool {
    return lhs.base() == rhs.base();
}

template<class T, class Pointer1, class Pointer2, class Reference1,
    class Reference2, class MapIterator, size_t SubarraySize>
[[nodiscard]] auto operator!=(const deque_iterator<T, Pointer1, Reference1, MapIterator, SubarraySize>& lhs,
                              const deque_iterator<T, Pointer2, Reference2, MapIterator, SubarraySize>& rhs) -> bool {
    return !(lhs.base() == rhs.base());
}

template<class T, class Pointer1, class Pointer2, class Reference1,
    class Reference2, class MapIterator, size_t SubarraySize>
[[nodiscard]] auto operator-(const deque_iterator<T, Pointer1, Reference1, MapIterator, SubarraySize>& lhs,
                             const deque_iterator<T, Pointer2, Reference2, MapIterator, SubarraySize>& rhs)
    -> typename iterator_traits<deque_iterator<T, Pointer1, Reference1, MapIterator, SubarraySize>>::difference_type {
    return SubarraySize * (distance(rhs.node_, lhs.node_) - 1) +
                (lhs.base() - lhs.start()) + (rhs.finish() - rhs.base());
}

template<class T, class Allocator = allocator<T>, size_t SubarraySize = deque_subarray_size<T>()>
class deque {

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

private:
    using alloc_traits           = allocator_traits<allocator_type>;
    using subarray_type          = unique_ptr<value_type, allocator_destructor<allocator_type>>;

public:
    using iterator               = deque_iterator<value_type, pointer, reference,
                                    typename list<subarray_type, allocator_type>::iterator, SubarraySize>;
    using const_iterator         = deque_iterator<value_type, const_pointer, const_reference,
                                    typename list<subarray_type, allocator_type>::iterator, SubarraySize>;

    using reverse_iterator       = ciel::reverse_iterator<iterator>;
    using const_reverse_iterator = ciel::reverse_iterator<const_iterator>;

private:
    [[no_unique_address]] allocator_type allocator_;
    list<subarray_type, allocator_type> map_;
    iterator begin_;
    iterator end_;

    auto alloc_range_destroy(iterator begin, iterator end) noexcept -> iterator {
        while (end != begin) {
            alloc_traits::destroy(allocator_, (--end).base());
        }
        return end;
    }

    template<class... Arg>
    auto alloc_range_construct_n(iterator begin, const size_type n, Arg&& ... arg) -> iterator {
        iterator end = begin;
        CIEL_TRY {
            for (size_type i = 0; i < n; ++i) {
                alloc_traits::construct(allocator_, end.base(), std::forward<Arg>(arg)...);
                ++end;
            }
            return end;
        } CIEL_CATCH (...) {
            alloc_range_destroy(begin, end);
            CIEL_THROW;
        }
    }

    template<class Iter>
        requires is_forward_iterator<Iter>::value
    auto alloc_range_construct(iterator begin, Iter first, Iter last) -> iterator {
        iterator end = begin;
        CIEL_TRY {
            while (first != last) {
                alloc_traits::construct(allocator_, end.base(), *first++);
                ++end;
            }
            return end;
        } CIEL_CATCH (...) {
            alloc_range_destroy(begin, end);
            CIEL_THROW;
        }
    }

    auto clear_and_get_cap_no_less_than(const size_type size) -> void {
        clear();
        begin_ = iterator(map_.front().get(), map_.begin());
        end_ = begin_;
        size_type s = size / SubarraySize + 1;
        if (map_.size() < s) {
            s -= map_.size();
            while (s-- > 0) {
                map_.emplace_back(subarray_type(alloc_traits::allocate(allocator_, SubarraySize), allocator_));
            }
        }
    }

    [[nodiscard]] auto front_space() const noexcept -> size_type {
        size_type res = begin_.cur_ - begin_.start();
        auto cur = begin_.node_;
        while (cur != map_.begin()) {
            res += SubarraySize;
            --cur;
        }
        return res;
    }

    [[nodiscard]] auto back_space() const noexcept -> size_type {
        size_type res = end_.finish() - end_.cur_ - 1;
        auto cur = end_.node_;
        while (cur != map_.end().prev()) {
            res += SubarraySize;
            ++cur;
        }
        return res;
    }

    auto alloc_range_nothrow_move(iterator begin, iterator first, iterator last) noexcept -> iterator {
        iterator end = begin;
        iterator boundary = first;
        while (first != last && end != boundary) {
            alloc_traits::construct(allocator_, (end++).base(), std::move(*first++));
        }
        while (first != last) {
            *begin++ = std::move(*first++);
        }
        return begin;
    }

    auto alloc_range_nothrow_move_backward(iterator end, iterator first, iterator last) noexcept -> iterator {
        iterator begin = end;
        iterator boundary = last;
        while (first != last && begin != boundary) {
            alloc_traits::construct(allocator_, (--begin).base(), std::move(*--last));
        }
        while (first != last) {
            *--begin = std::move(*--last);
        }
        return end;
    }

    // Check which endpoint pos close to
    template<class Arg>
    auto insert_n(iterator pos, const size_type count, Arg&& arg) -> iterator {
        if (!count) {
            return pos;
        }
        if (distance(begin(), pos) < distance(pos, end())) {  // move left half to left
            if (size_type bs = front_space(); count > bs) {
                bs = (count - bs) / SubarraySize + 1;
                while (bs-- > 0) {
                    map_.emplace_front(subarray_type(alloc_traits::allocate(allocator_, SubarraySize), allocator_));
                }
            }

            iterator boundary = begin_;
            iterator res = pos - 1;
            begin_ = alloc_range_nothrow_move(begin_ - count, begin_, pos);

            const size_type left_movement = pos - boundary;
            size_type loop_break = left_movement < count ? left_movement : count;
            for (size_type i = 0; i < loop_break; ++i) {    // assignments
                *--pos = std::forward<Arg>(arg);
            }
            if (left_movement < count) {    // After assignments, there are elements need to be constructed
                alloc_range_construct_n(begin_ + loop_break, count - left_movement, std::forward<Arg>(arg));
            }
            return res;
        } else {    // move right half to right
            if (size_type bs = back_space(); count >= bs) {
                bs = (count - bs) / SubarraySize + 1;
                while (bs-- > 0) {
                    map_.emplace_back(subarray_type(alloc_traits::allocate(allocator_, SubarraySize), allocator_));
                }
            }

            iterator boundary = end_;
            end_ = alloc_range_nothrow_move_backward(end_ + count, pos, end_);
            const size_type left_movement = boundary - pos;
            const size_type loop_break = left_movement < count ? left_movement : count;
            for (size_type i = 0; i < loop_break; ++i) {    // assignments
                *(pos++) = std::forward<Arg>(arg);
            }
            if (left_movement < count) {    // After assignments, there are elements need to be constructed
                pos = alloc_range_construct_n(boundary, count - left_movement, std::forward<Arg>(arg)) - 1;
            }
            return pos;
        }
    }

public:
    deque()
        : allocator_(), map_(allocator_) {
        map_.emplace_back(subarray_type(alloc_traits::allocate(allocator_, SubarraySize), allocator_));
        begin_ = iterator(map_.front().get(), map_.begin());
        end_ = begin_;
    }

    explicit deque(const allocator_type& alloc)
        : allocator_(alloc), map_(allocator_) {
        map_.emplace_back(subarray_type(alloc_traits::allocate(allocator_, SubarraySize), allocator_));
        begin_ = iterator(map_.front().get(), map_.begin());
        end_ = begin_;
    }

    deque(const size_type count, const T& value, const allocator_type& alloc = allocator_type())
        : deque(alloc) {
        size_type s = count / SubarraySize;
        while (s-- > 0) {
            map_.emplace_back(subarray_type(alloc_traits::allocate(allocator_, SubarraySize), allocator_));
        }
        end_ = alloc_range_construct_n(begin_, count, value);
    }

    explicit deque(const size_type count, const allocator_type& alloc = allocator_type())
        : deque(alloc) {
        size_type s = count / SubarraySize;
        while (s-- > 0) {
            map_.emplace_back(subarray_type(alloc_traits::allocate(allocator_, SubarraySize), allocator_));
        }
        end_ = alloc_range_construct_n(begin_, count);
    }

    template<class Iter>
        requires is_exactly_input_iterator<Iter>::value
    deque(Iter first, Iter last, const allocator_type& alloc = allocator_type())
        : deque(alloc) {
        CIEL_TRY {
            while (first != last) {
                emplace_back(*first);
                ++first;
            }
        } CIEL_CATCH (...) {
            clear();
            CIEL_THROW;
        }
    }

    template<class Iter>
        requires is_forward_iterator<Iter>::value
    deque(Iter first, Iter last, const allocator_type& alloc = allocator_type())
        : deque(alloc) {
        auto s = ciel::distance(first, last) / SubarraySize;
        while (s-- > 0) {
            map_.emplace_back(subarray_type(alloc_traits::allocate(allocator_, SubarraySize), allocator_));
        }
        end_ = alloc_range_construct(begin_, first, last);
    }

    deque(const deque& other)
        : deque(other.begin(), other.end(),
                alloc_traits::select_on_container_copy_construction(other.get_allocator())) {}

    deque(const deque& other, const allocator_type& alloc)
        : deque(other.begin(), other.end(), alloc) {}

    deque(deque&& other) noexcept
        : deque() {
        swap(other);
    }

    deque(deque&& other, const allocator_type& alloc)
        : deque() {
        if (alloc == other.get_allocator()) {
            swap(other);
            allocator_ = alloc;
        } else {
            deque(other, alloc).swap(*this);
        }
    }

    deque(std::initializer_list<T> init, const allocator_type& alloc = allocator_type())
        : deque(init.begin(), init.end(), alloc) {}

    ~deque() {
        clear();
    }

    auto operator=(const deque& other) -> deque& {
        if (this == addressof(other)) {
            return *this;
        }
        if (alloc_traits::propagate_on_container_copy_assignment::value) {
            if (allocator_ != other.allocator_) {
                deque(other.allocator_).swap(*this);
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

    auto operator=(deque&& other) noexcept(alloc_traits::is_always_equal::value) -> deque& {
        if (this == addressof(other)) {
            return *this;
        }
        if (!alloc_traits::propagate_on_container_move_assignment::value && allocator_ != other.allocator_) {
            clear_and_get_cap_no_less_than(other.size());
            end_ = alloc_range_construct(begin_, other.begin(), other.end());
            return *this;
        }
        if (alloc_traits::propagate_on_container_move_assignment::value) {
            allocator_ = std::move(other.allocator_);
        }
        clear();
        swap(other);
        return *this;
    }

    auto operator=(std::initializer_list<T> ilist) -> deque& {
        clear_and_get_cap_no_less_than(ilist.size());
        end_ = alloc_range_construct(begin_, ilist.begin(), ilist.end());
        return *this;
    }

    auto assign(const size_type count, const T& value) -> void {
        clear_and_get_cap_no_less_than(count);
        end_ = alloc_range_construct_n(begin_, count, value);
    }

    template<class Iter>
        requires is_exactly_input_iterator<Iter>::value
    auto assign(Iter first, Iter last) -> void {
        clear();
        begin_ = iterator(map_.front().get(), map_.begin());
        end_ = begin_;
        while (first != last) {
            emplace_back(*first);
            ++first;
        }
    }

    template<class Iter>
        requires is_forward_iterator<Iter>::value
    auto assign(Iter first, Iter last) -> void {
        clear_and_get_cap_no_less_than(ciel::distance(first, last));
        end_ = alloc_range_construct(begin_, first, last);
    }

    auto assign(std::initializer_list<T> ilist) -> void {
        clear_and_get_cap_no_less_than(ilist.size());
        end_ = alloc_range_construct(begin_, ilist.begin(), ilist.end());
    }

    [[nodiscard]] auto get_allocator() const noexcept -> allocator_type {
        return allocator_;
    }

    [[nodiscard]] auto at(const size_type pos) -> reference {
        if (pos >= size()) {
            THROW(std::out_of_range("pos is not within the range of ciel::deque"));
        }
        return (*this)[pos];
    }

    [[nodiscard]] auto at(const size_type pos) const -> const_reference {
        if (pos >= size()) {
            THROW(std::out_of_range("pos is not within the range of ciel::deque"));
        }
        return (*this)[pos];
    }

    [[nodiscard]] auto operator[](const size_type pos) -> reference {
        return *(begin_ + pos);
    }

    [[nodiscard]] auto operator[](const size_type pos) const -> const_reference {
        return *(begin_ + pos);
    }

    [[nodiscard]] auto front() -> reference {
        return *begin();
    }

    [[nodiscard]] auto front() const -> const_reference {
        return *begin();
    }

    [[nodiscard]] auto back() -> reference {
        return *prev(end());
    }

    [[nodiscard]] auto back() const -> const_reference {
        return *prev(end());
    }

    [[nodiscard]] auto begin() noexcept -> iterator {
        return begin_;
    }

    [[nodiscard]] auto begin() const noexcept -> const_iterator {
        return begin_;
    }

    [[nodiscard]] auto cbegin() const noexcept -> const_iterator {
        return begin();
    }

    [[nodiscard]] auto end() noexcept -> iterator {
        return end_;
    }

    [[nodiscard]] auto end() const noexcept -> const_iterator {
        return end_;
    }

    [[nodiscard]] auto cend() const noexcept -> const_iterator {
        return end();
    }

    [[nodiscard]] auto rbegin() noexcept -> reverse_iterator {
        return reverse_iterator(end());
    }

    [[nodiscard]] auto rbegin() const noexcept -> const_reverse_iterator {
        return const_reverse_iterator(end());
    }

    [[nodiscard]] auto crbegin() const noexcept -> const_reverse_iterator {
        return rbegin();
    }

    [[nodiscard]] auto rend() noexcept -> reverse_iterator {
        return reverse_iterator(begin());
    }

    [[nodiscard]] auto rend() const noexcept -> const_reverse_iterator {
        return const_reverse_iterator(begin());
    }

    [[nodiscard]] auto crend() const noexcept -> const_reverse_iterator {
        return rend();
    }

    [[nodiscard]] auto empty() const noexcept -> bool {
        return begin() == end();
    }

    [[nodiscard]] auto size() const noexcept -> size_type {
        return distance(begin(), end());
    }

    [[nodiscard]] auto max_size() const noexcept -> size_type {
        return numeric_limits<difference_type>::max();
    }

    auto shrink_to_fit() -> void {
        while (map_.begin() != begin_.node_) {
            map_.pop_front();
        }
        while (map_.end().prev() != end_.node_) {
            map_.pop_back();
        }
    }

    auto clear() noexcept -> void {
        end_ = alloc_range_destroy(begin(), end());
    }

    auto insert(const_iterator pos, const T& value) -> iterator {
        return insert_n(pos, 1, value);
    }

    auto insert(const_iterator pos, T&& value) -> iterator {
        return insert_n(pos, 1, std::move(value));
    }

    auto insert(const_iterator pos, const size_type count, const T& value) -> iterator {
        return insert_n(pos, count, value);
    }

    template<class Iter>
        requires is_exactly_input_iterator<Iter>::value
    auto insert(const_iterator pos, Iter first, Iter last) -> iterator {
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
    auto insert(const_iterator pos, Iter first, Iter last) -> iterator {
        size_type count = distance(first, last);
        if (!count) {
            return pos;
        }
        auto front_distance = distance(begin(), iterator(pos));
        auto back_distance = distance(iterator(pos), end());
        if (front_distance < back_distance) {  // move left half to left
            if (size_type bs = front_space(); count > bs) {
                bs = (count - bs) / SubarraySize + 1;
                while (bs--) {
                    map_.emplace_front(subarray_type(alloc_traits::allocate(allocator_, SubarraySize), allocator_));
                }
            }

            iterator boundary = begin_;
            iterator res = pos - 1;
            begin_ = alloc_range_nothrow_move(begin_ - count, begin_, pos);

            iterator insert_begin = begin_ + front_distance;

            size_type left_movement = pos - boundary;

            size_type loop_break = left_movement < count ? left_movement : count;
            if (left_movement < count) {
                insert_begin = alloc_range_construct(insert_begin, first, first + count - left_movement);
            }
            copy_n(first + count - left_movement, loop_break, insert_begin);
            return res;
        } else {    // move right half to right
            if (size_type bs = back_space(); count >= bs) {
                bs = (count - bs) / SubarraySize + 1;
                while (bs--) {
                    map_.emplace_back(subarray_type(alloc_traits::allocate(allocator_, SubarraySize), allocator_));
                }
            }

            iterator boundary = end_;
            end_ = alloc_range_nothrow_move_backward(end_ + count, pos, end_);
            size_type left_movement = boundary - pos;
            size_type loop_break = left_movement < count ? left_movement : count;
            copy_n(first, loop_break, iterator(pos));
            if (left_movement < count) {    // After assignments, there are elements need to be constructed
                pos = alloc_range_construct(boundary, first + left_movement, last) - 1;
            }
            return pos;
        }
    }

    auto insert(const_iterator pos, std::initializer_list<T> ilist) -> iterator {
        return insert(pos, ilist.begin(), ilist.end());
    }

    template<class... Args>
    auto emplace(const_iterator pos, Args&&... args) -> iterator {
        if (pos == begin()) {
            emplace_front(std::forward<Args>(args)...);
            return begin();
        }
        if (pos == end()) {
            emplace_back(std::forward<Args>(args)...);
            return iterator(pos);
        }
        return insert_n(pos, 1, value_type(std::forward<Args>(args)...));
    }

    auto erase(const_iterator pos) -> iterator {
        return erase(pos, pos + 1);
    }

    auto erase(const_iterator first, const_iterator last) -> iterator {
        auto distance = ciel::distance(first, last);
        if (!distance) {
            return last;
        }
        if (ciel::distance(begin(), iterator(first)) < ciel::distance(iterator(last), end())) {
            iterator old_begin = begin_;
            begin_ = move_backward(begin(), iterator(first), iterator(last));
            alloc_range_destroy(old_begin, begin_);
        } else {
            iterator new_end = move(iterator(last), end(), iterator(first));
            end_ = alloc_range_destroy(new_end, end_);
        }
        return end();
    }

    auto push_back(const T& value) -> void {
        emplace_back(value);
    }

    auto push_back(T&& value) -> void {
        emplace_back(std::move(value));
    }

    template<class... Args>
    auto emplace_back(Args&& ... args) -> reference {
        // Don't do end_.cur_ + 1 == end_.end(), because end() will jump to the head of next subarray
        if (end_.cur_ == end_.finish() - 1 && end_.node_ == map_.end().prev()) {
            map_.emplace_back(subarray_type(alloc_traits::allocate(allocator_, SubarraySize), allocator_));
        }
        end_ = alloc_range_construct_n(end_, 1, std::forward<Args>(args)...);
        return back();
    }

    auto pop_back() noexcept -> void {
        end_ = alloc_range_destroy(end_ - 1, end_);
    }

    auto push_front(const T& value) -> void {
        emplace_front(value);
    }

    auto push_front(T&& value) -> void {
        emplace_front(std::move(value));
    }

    template<class... Args>
    auto emplace_front(Args&& ... args) -> reference {
        if (begin_.cur_ == begin_.start() && begin_.node_ == map_.begin()) {
            map_.emplace_front(subarray_type(alloc_traits::allocate(allocator_, SubarraySize), allocator_));
        }
        alloc_range_construct_n(begin_ - 1, 1, std::forward<Args>(args)...);
        --begin_;
        return front();
    }

    auto pop_front() noexcept -> void {
        alloc_range_destroy(begin_, begin_ + 1);
        ++begin_;
    }

    auto resize(const size_type count) -> void {
        if (size() >= count) {
            end_ = alloc_range_destroy(end_ - (size() - count), end_);
        } else {
            const size_type needed_space = count - size();
            if (size_type bs = back_space(); needed_space >= bs) {
                bs = (needed_space - bs) / SubarraySize + 1;
                while (bs-- > 0) {
                    map_.emplace_back(subarray_type(alloc_traits::allocate(allocator_, SubarraySize), allocator_));
                }
            }
            end_ = alloc_range_construct_n(end_, needed_space);
        }
    }

    auto resize(const size_type count, const value_type& value) -> void {
        if (size() >= count) {
            end_ = alloc_range_destroy(end_ - (size() - count), end_);
        } else {
            size_type needed_space = count - size();
            if (size_type bs = back_space(); needed_space >= bs) {
                bs = (needed_space - bs) / SubarraySize + 1;
                while (bs-- > 0) {
                    map_.emplace_back(subarray_type(alloc_traits::allocate(allocator_, SubarraySize), allocator_));
                }
            }
            end_ = alloc_range_construct_n(end_, needed_space, value);
        }
    }

    auto swap(deque& other) noexcept(alloc_traits::is_always_equal::value) -> void {
        using std::swap;
        swap(map_, other.map_);
        swap(begin_, other.begin_);
        swap(end_, other.end_);
        swap(allocator_, other.allocator_);
    }

};    // class deque

template<class T, class Alloc>
[[nodiscard]] constexpr auto operator==(const deque<T, Alloc>& lhs, const deque<T, Alloc>& rhs) -> bool {
    if (lhs.size() != rhs.size()) {
        return false;
    }
    return equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<class T, class Alloc, class U>
auto erase(deque<T, Alloc>& c, const U& value) -> typename deque<T, Alloc>::size_type {
    auto it = ciel::remove(c.begin(), c.end(), value);
    const auto r = distance(it, c.end());
    c.erase(it, c.end());
    return r;
}

template<class T, class Alloc, class Pred>
auto erase_if(deque<T, Alloc>& c, Pred pred) -> typename deque<T, Alloc>::size_type {
    auto it = ciel::remove_if(c.begin(), c.end(), pred);
    const auto r = distance(it, c.end());
    c.erase(it, c.end());
    return r;
}

template<legacy_input_iterator Iter, class Alloc = allocator<typename iterator_traits<Iter>::value_type>>
deque(Iter, Iter, Alloc = Alloc()) -> deque<typename iterator_traits<Iter>::value_type, Alloc>;

NAMESPACE_CIEL_END

namespace std {

template<class T, class Alloc>
auto swap(ciel::deque<T, Alloc>& lhs, ciel::deque<T, Alloc>& rhs) noexcept(noexcept(lhs.swap(rhs))) -> void {
    lhs.swap(rhs);
}

}   // namespace std

#endif // CIELUTILS_INCLUDE_CIEL_DEQUE_HPP_