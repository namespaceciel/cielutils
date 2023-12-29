#ifndef CIELUTILS_INCLUDE_CIEL_DEQUE_HPP_
#define CIELUTILS_INCLUDE_CIEL_DEQUE_HPP_

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

// Differences between std::deque and this class
// To be simple, the "map" of deque (control block of every subarray) is list-implemented,
//      while the common implemention is vector-like. It can avoid reallocations.
//      But we can't get negative distance of two iterators instead.
// Also, to be simple, empty deque will still allocate a block of memory, and if last block of subarray is full,
//      end() will be at the first of a new-allocated subarray
// T is not allowed to throw in move constructor/assignment
// emplace, insert and resize has strong exception safety

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
                          const deque_iterator<U, Pointer2, Reference2, M, S>&) noexcept
        -> typename iterator_traits<deque_iterator<U, Pointer1, Reference1, M, S>>::difference_type;

public:
    [[nodiscard]] auto start() const noexcept -> T* {
        // Why node_->get() won't compile?
        //
        // https://en.cppreference.com/w/cpp/language/operator_member_access
        // If a user-defined operator-> is called, operator-> is called again on the resulting value,
        // recursively, until an operator-> is reached that returns a plain pointer.
        // After that, built-in semantics are applied to that pointer.
        //
        // And node_ we have here is typename list<unique_ptr<T>>::iterator,
        // so first -> will get unique_ptr<T>* ->get(),
        // second -> will get T* ->get(),
        // third -> will get T.get(), and we don't want that result :(

        return (*node_).get();
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
        ++(*this);
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
        --(*this);
        return res;
    }

    auto operator+=(const difference_type n) noexcept -> deque_iterator& {
        difference_type offset = n + (cur_ - start());
        if (offset >= 0 && static_cast<size_t>(offset) < SubarraySize) {
            cur_ += n;
        } else {
            advance(node_, offset > 0 ? offset / static_cast<difference_type>(SubarraySize) :
                                     (offset + 1) / static_cast<difference_type>(SubarraySize) - 1);
            offset %= static_cast<difference_type>(SubarraySize);
            if (offset < 0) {
                offset += static_cast<difference_type>(SubarraySize);
            }

            CIEL_POSTCONDITION(offset >= 0);

            cur_ = start() + offset;
        }
        return *this;
    }

    auto operator-=(const difference_type n) noexcept -> deque_iterator& {
        return (*this) += (-n);
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
                    const deque_iterator<T, Pointer2, Reference2, MapIterator, SubarraySize>& rhs) noexcept -> bool {
    return lhs.base() == rhs.base();
}

template<class T, class Pointer1, class Pointer2, class Reference1,
         class Reference2, class MapIterator, size_t SubarraySize>
[[nodiscard]] auto operator!=(const deque_iterator<T, Pointer1, Reference1, MapIterator, SubarraySize>& lhs,
                    const deque_iterator<T, Pointer2, Reference2, MapIterator, SubarraySize>& rhs) noexcept -> bool {
    return !(lhs.base() == rhs.base());
}

template<class T, class Pointer1, class Pointer2, class Reference1,
         class Reference2, class MapIterator, size_t SubarraySize>
[[nodiscard]] auto operator-(const deque_iterator<T, Pointer1, Reference1, MapIterator, SubarraySize>& lhs,
                             const deque_iterator<T, Pointer2, Reference2, MapIterator, SubarraySize>& rhs) noexcept
    -> typename iterator_traits<deque_iterator<T, Pointer1, Reference1, MapIterator, SubarraySize>>::difference_type {

    // We need to make sure lhs >= rhs, otherwise distance(rhs.node_, lhs.node_) may crash,
    // since the node_ is list iterator and can't get negative result out of distance function

    return SubarraySize * (distance(rhs.node_, lhs.node_) - 1) +
                (lhs.base() - lhs.start()) + (rhs.finish() - rhs.base());
}

template<class T, class Allocator = allocator<T>, size_t SubarraySize = deque_subarray_size<T>()>
class deque {

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
    size_type begin_offset_;
    size_type size_;

    auto alloc_range_destroy(iterator begin, iterator end) noexcept -> void {
        while (begin != end) {
            alloc_traits::destroy(allocator_, (begin++).base());
            --size_;
        }
    }

    template<class... Args>
    auto alloc_range_construct_n(iterator begin, const size_type n, Args&& ... args) -> iterator {
        iterator end = begin;

        CIEL_TRY {
            for (size_type i = 0; i < n; ++i) {
                alloc_traits::construct(allocator_, end.base(), std::forward<Args>(args)...);
                ++end;
                ++size_;
            }
            return end;

        } CIEL_CATCH (...) {
            alloc_range_destroy(begin, end);
            CIEL_THROW;
        }
    }

    template<class Iter>
    auto alloc_range_construct(iterator begin, Iter first, Iter last) -> iterator {
        iterator end = begin;

        CIEL_TRY {
            while (first != last) {
                alloc_traits::construct(allocator_, end.base(), *first);
                ++first;
                ++end;
                ++size_;
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

    [[nodiscard]] auto front_spare() const noexcept -> size_type {
        return begin_offset_;
    }

    [[nodiscard]] auto back_spare() const noexcept -> size_type {
        size_type res = end().finish() - end().cur_ - 1;
        auto cur = end().node_;
        while (cur != map_.end().prev()) {
            res += SubarraySize;
            ++cur;
        }
        return res;
    }

    // Check which endpoint is pos close to
    template<class Arg>
    auto insert_n(iterator pos, const size_type count, Arg&& arg) -> iterator {
        if (count == 0) {
            return pos;
        }

        if (distance(begin(), pos) < distance(pos, end())) {    // move left half to left

            get_enough_front_spare(count);

            CIEL_PRECONDITION(front_spare() >= count);

            alloc_range_construct_n(begin() - count, count, std::forward<Arg>(arg));

            iterator old_begin = begin();
            begin_offset_ -= count;

            rotate(begin(), old_begin, pos);
            return --pos;

        } else {    // move right half to right

            get_enough_back_spare(count);

            CIEL_PRECONDITION(back_spare() >= count);

            iterator old_end = end();
            alloc_range_construct_n(end(), count, std::forward<Arg>(arg));

            return rotate(pos, old_end, end()) - 1;
        }
    }

    auto get_enough_front_spare(const size_type count) -> void {
        if (size_type fs = front_spare(); count > fs) {
            fs = (count - fs - 1) / SubarraySize + 1;   // blocks of memory we need

            // Check if we can use back spare
            size_type bs = back_spare();
            while (bs >= SubarraySize && fs > 0) {
                auto block = std::move(map_.back());
                map_.pop_back();
                map_.emplace_front(std::move(block));
                bs -= SubarraySize;

                begin_offset_ += SubarraySize;
                --fs;
            }

            while (fs > 0) {
                map_.emplace_front(subarray_type(alloc_traits::allocate(allocator_, SubarraySize), allocator_));

                begin_offset_ += SubarraySize;
                --fs;
            }
        }
    }

    auto get_enough_back_spare(const size_type count) -> void {
        if (size_type bs = back_spare(); count > bs) {
            bs = (count - bs - 1) / SubarraySize + 1;

            // Check if we can use front spare
            size_type fs = front_spare();
            while (fs >= SubarraySize && bs > 0) {
                auto block = std::move(map_.front());
                map_.pop_front();
                begin_offset_ -= SubarraySize;

                map_.emplace_back(std::move(block));
                fs -= SubarraySize;

                --bs;
            }

            while (bs > 0) {
                map_.emplace_back(subarray_type(alloc_traits::allocate(allocator_, SubarraySize), allocator_));

                --bs;
            }
        }
    }

public:
    deque()
        : allocator_(), map_(allocator_), begin_offset_(0), size_(0) {
        map_.emplace_back(subarray_type(alloc_traits::allocate(allocator_, SubarraySize), allocator_));
    }

    explicit deque(const allocator_type& alloc)
        : allocator_(alloc), map_(allocator_), begin_offset_(0), size_(0) {
        map_.emplace_back(subarray_type(alloc_traits::allocate(allocator_, SubarraySize), allocator_));
    }

    deque(const size_type count, const T& value, const allocator_type& alloc = allocator_type())
        : deque(alloc) {
        size_type s = count / SubarraySize;
        while (s-- > 0) {
            map_.emplace_back(subarray_type(alloc_traits::allocate(allocator_, SubarraySize), allocator_));
        }
        alloc_range_construct_n(begin(), count, value);
    }

    explicit deque(const size_type count, const allocator_type& alloc = allocator_type())
        : deque(alloc) {
        size_type s = count / SubarraySize;
        while (s-- > 0) {
            map_.emplace_back(subarray_type(alloc_traits::allocate(allocator_, SubarraySize), allocator_));
        }
        alloc_range_construct_n(begin(), count);
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
        alloc_range_construct(begin(), first, last);
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
                assign(other.begin(), other.end());
                return *this;
            }

            allocator_ = other.allocator_;
        }
        assign(other.begin(), other.end());
        return *this;
    }

    auto operator=(deque&& other) noexcept(alloc_traits::is_always_equal::value) -> deque& {
        if (this == addressof(other)) {
            return *this;
        }

        if (!alloc_traits::propagate_on_container_move_assignment::value && allocator_ != other.allocator_) {
            assign(other.begin(), other.end());
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
        assign(ilist.begin(), ilist.end());
        return *this;
    }

    auto assign(const size_type count, const T& value) -> void {
        if (back_spare() + size() < count) {
            get_enough_back_spare(count - size());

            CIEL_POSTCONDITION(back_spare() + size() >= count);
        } else if (size() > count) {
            alloc_range_destroy(begin() + count, end());;
        }

        CIEL_POSTCONDITION(size() <= count);

        range_assign_n(begin(), size(), value);
        // if count > size()
        alloc_range_construct_n(end(), count - size(), value);

        CIEL_POSTCONDITION(size() == count);
    }

    template<class Iter>
        requires is_exactly_input_iterator<Iter>::value
    auto assign(Iter first, Iter last) -> void {
        clear();
        begin_offset_ = 0;
        while (first != last) {
            emplace_back(*first);
            ++first;
        }
    }

    template<class Iter>
        requires is_forward_iterator<Iter>::value
    auto assign(Iter first, Iter last) -> void {
        const size_type count = ciel::distance(first, last);

        if (back_spare() + size() < count) {
            get_enough_back_spare(count - size());

            CIEL_POSTCONDITION(back_spare() + size() >= count);
        } else if (size() > count) {
            alloc_range_destroy(begin() + count, end());;
        }

        CIEL_POSTCONDITION(size() <= count);

        Iter mid = first + size();

        range_assign(begin(), first, mid);
        // if mid < last
        alloc_range_construct(end(), mid, last);

        CIEL_POSTCONDITION(size() == count);
    }

    auto assign(std::initializer_list<T> ilist) -> void {
        assign(ilist.begin(), ilist.end());
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
        CIEL_PRECONDITION(pos < size());

        return *(begin() + pos);
    }

    [[nodiscard]] auto operator[](const size_type pos) const -> const_reference {
        CIEL_PRECONDITION(pos < size());

        return *(begin() + pos);
    }

    [[nodiscard]] auto front() -> reference {
        CIEL_PRECONDITION(!empty());

        return *begin();
    }

    [[nodiscard]] auto front() const -> const_reference {
        CIEL_PRECONDITION(!empty());

        return *begin();
    }

    [[nodiscard]] auto back() -> reference {
        CIEL_PRECONDITION(!empty());

        return *prev(end());
    }

    [[nodiscard]] auto back() const -> const_reference {
        CIEL_PRECONDITION(!empty());

        return *prev(end());
    }

    [[nodiscard]] auto begin() noexcept -> iterator {
        return iterator(map_.front().get(), map_.begin()) + begin_offset_;
    }

    [[nodiscard]] auto begin() const noexcept -> const_iterator {
        return iterator(map_.front().get(), map_.begin()) + begin_offset_;
    }

    [[nodiscard]] auto cbegin() const noexcept -> const_iterator {
        return begin();
    }

    [[nodiscard]] auto end() noexcept -> iterator {
        return begin() + size();
    }

    [[nodiscard]] auto end() const noexcept -> const_iterator {
        return begin() + size();
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
        return size() == 0;
    }

    [[nodiscard]] auto size() const noexcept -> size_type {
        return size_;
    }

    [[nodiscard]] auto max_size() const noexcept -> size_type {
        return alloc_traits::max_size(allocator_);
    }

    auto shrink_to_fit() noexcept -> void {
        while (begin_offset_ >= SubarraySize) {
            map_.pop_front();
            begin_offset_ -= SubarraySize;
        }

        auto bs = back_spare();
        while (bs >= SubarraySize) {
            map_.pop_back();
            bs -= SubarraySize;
        }
    }

    auto clear() noexcept -> void {
        alloc_range_destroy(begin(), end());
    }

    auto insert(iterator pos, const value_type& value) -> iterator {
        return insert_n(pos, 1, value);
    }

    auto insert(iterator pos, value_type&& value) -> iterator {
        return insert_n(pos, 1, std::move(value));
    }

    auto insert(iterator pos, const size_type count, const value_type& value) -> iterator {
        return insert_n(pos, count, value);
    }

    template<class Iter>
        requires is_exactly_input_iterator<Iter>::value
    auto insert(iterator pos, Iter first, Iter last) -> iterator {

        auto old_end = end();

        CIEL_TRY {
            while (first != last) {
                emplace_back(*first);
                ++first;
            }
        } CIEL_CATCH (...) {
            alloc_range_destroy(old_end, end());
            CIEL_THROW;
        }
        return rotate(pos, old_end, end()) - 1;
    }

    template<class Iter>
        requires is_forward_iterator<Iter>::value
    auto insert(iterator pos, Iter first, Iter last) -> iterator {
        const auto count = distance(first, last);
        if (count <= 0) {
            return pos;
        }

        if (distance(begin(), pos) < distance(pos, end())) {  // move left half to left

            get_enough_front_spare(count);

            CIEL_PRECONDITION(front_spare() >= static_cast<size_type>(count));

            alloc_range_construct(begin() - count, first, last);

            iterator old_begin = begin();
            begin_offset_ -= count;

            rotate(begin(), old_begin, pos);
            return --pos;

        } else {    // move right half to right

            get_enough_back_spare(count);

            CIEL_PRECONDITION(back_spare() >= static_cast<size_type>(count));

            iterator old_end = end();
            alloc_range_construct(end(), first, last);

            return rotate(pos, old_end, end()) - 1;
        }
    }

    auto insert(iterator pos, std::initializer_list<T> ilist) -> iterator {
        return insert(pos, ilist.begin(), ilist.end());
    }

    template<class... Args>
    auto emplace(iterator pos, Args&&... args) -> iterator {
        if (pos == begin()) {
            emplace_front(std::forward<Args>(args)...);
            return begin();
        }
        if (pos == end()) {
            emplace_back(std::forward<Args>(args)...);
            return pos;
        }
        return insert_n(pos, 1, value_type(std::forward<Args>(args)...));
    }

    auto erase(iterator pos) noexcept -> iterator {
        CIEL_PRECONDITION(!empty());

        return erase(pos, pos + 1);
    }

    auto erase(iterator first, iterator last) noexcept -> iterator {
        const difference_type begin_first_distance = distance(begin(), first);
        const difference_type begin_last_distance = distance(begin(), last);

        // Don't do count = distance(first, last) because if last is at the left subarray of first's,
        //
        // ----------   ----------   ----------   ----------  (list<unique_ptr<T>>)
        //     |              |            |              |
        //   begin()        last         first           end()
        //
        // we can't get negative result out of distance function on list's iterator

        const difference_type count = begin_last_distance - begin_first_distance;
        if (count <= 0) {
            return last;
        }

        if (begin_first_distance < distance(last, end())) {
            move_backward(begin(), first, last);

            iterator old_begin = begin();
            begin_offset_ += count;

            alloc_range_destroy(old_begin, begin());

        } else {
            iterator new_end = move(last, end(), first);

            alloc_range_destroy(new_end, end());
        }

        return begin() + begin_first_distance;
    }

    auto push_back(const value_type& value) -> void {
        emplace_back(value);
    }

    auto push_back(value_type&& value) -> void {
        emplace_back(std::move(value));
    }

    template<class... Args>
    auto emplace_back(Args&& ... args) -> reference {
        get_enough_back_spare(1);
        alloc_range_construct_n(end(), 1, std::forward<Args>(args)...);
        return back();
    }

    auto pop_back() noexcept -> void {
        CIEL_PRECONDITION(!empty());

        alloc_range_destroy(--end(), end());
    }

    auto push_front(const value_type& value) -> void {
        emplace_front(value);
    }

    auto push_front(value_type&& value) -> void {
        emplace_front(std::move(value));
    }

    template<class... Args>
    auto emplace_front(Args&& ... args) -> reference {
        get_enough_front_spare(1);

        CIEL_PRECONDITION(begin_offset_ > 0);

        alloc_range_construct_n(--begin(), 1, std::forward<Args>(args)...);
        --begin_offset_;

        return front();
    }

    auto pop_front() noexcept -> void {
        CIEL_PRECONDITION(!empty());

        alloc_range_destroy(begin(), ++begin());
        ++begin_offset_;
    }

    auto resize(const size_type count) -> void {
        if (size() >= count) {
            alloc_range_destroy(begin() + count, end());
        } else {
            const size_type needed_space = count - size();

            get_enough_back_spare(needed_space);

            alloc_range_construct_n(end(), needed_space);
        }
    }

    auto resize(const size_type count, const value_type& value) -> void {
        if (size() >= count) {
            alloc_range_destroy(begin() + count, end());
        } else {
            const size_type needed_space = count - size();

            get_enough_back_spare(needed_space);

            alloc_range_construct_n(end(), needed_space, value);
        }
    }

    auto swap(deque& other) noexcept(alloc_traits::is_always_equal::value) -> void {
        using std::swap;
        swap(map_, other.map_);
        swap(begin_offset_, other.begin_offset_);
        swap(size_, other.size_);
        swap(allocator_, other.allocator_);
    }

};    // class deque

template<class T, class Alloc, size_t Size1, size_t Size2>
[[nodiscard]] constexpr auto operator==(const deque<T, Alloc, Size1>& lhs, const deque<T, Alloc, Size2>& rhs) -> bool {
    if (lhs.size() != rhs.size()) {
        return false;
    }
    return equal(lhs.begin(), lhs.end(), rhs.begin());
}

// So that we can test more efficiently
template<class T, class Alloc, size_t Size>
[[nodiscard]] constexpr auto operator==(const deque<T, Alloc, Size>& lhs, std::initializer_list<T> rhs) -> bool {
    if (lhs.size() != rhs.size()) {
        return false;
    }
    return ciel::equal(lhs.begin(), lhs.end(), rhs.begin());
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

template<class Iter, class Alloc = allocator<typename iterator_traits<Iter>::value_type>>
deque(Iter, Iter, Alloc = Alloc()) -> deque<typename iterator_traits<Iter>::value_type, Alloc>;

NAMESPACE_CIEL_END

namespace std {

template<class T, class Alloc>
auto swap(ciel::deque<T, Alloc>& lhs, ciel::deque<T, Alloc>& rhs) noexcept(noexcept(lhs.swap(rhs))) -> void {
    lhs.swap(rhs);
}

}   // namespace std

#endif // CIELUTILS_INCLUDE_CIEL_DEQUE_HPP_