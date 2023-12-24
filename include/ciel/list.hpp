#ifndef CIELUTILS_INCLUDE_CIEL_LIST_HPP_
#define CIELUTILS_INCLUDE_CIEL_LIST_HPP_

#include <ciel/algorithm_impl/equal.hpp>
#include <ciel/config.hpp>
#include <ciel/iterator_impl/distance.hpp>
#include <ciel/iterator_impl/iterator_tag.hpp>
#include <ciel/iterator_impl/reverse_iterator.hpp>
#include <ciel/memory_impl/allocator.hpp>
#include <ciel/memory_impl/allocator_traits.hpp>

NAMESPACE_CIEL_BEGIN

// TODO: operator<=>, operations like merge and sort

struct list_node_base {
    list_node_base* prev_;
    list_node_base* next_;

    list_node_base() : prev_(this), next_(this) {}

    list_node_base(list_node_base* p, list_node_base* n) : prev_(p), next_(n) {}

    auto clear() noexcept -> void {
        prev_ = this;
        next_ = this;
    }

};    // struct list_node_base

template<class T>
struct list_node : list_node_base {
    T value_;

    template<class... Args>
    list_node(list_node_base* p, list_node_base* n, Args&& ... args)
        : list_node_base(p, n), value_(std::forward<Args>(args)...) {}

};    // struct list_node

template<class T, class Pointer, class Reference>
class list_iterator {
public:
    using difference_type   = ptrdiff_t;
    using value_type        = T;
    using pointer           = Pointer;
    using reference         = Reference;
    using iterator_category = bidirectional_iterator_tag;
    using iterator_concept  = bidirectional_iterator_tag;

private:
    using base_node_type    = list_node_base;
    using node_type         = list_node<value_type>;

    base_node_type* it_;

public:
    list_iterator() noexcept: it_(nullptr) {}

    explicit list_iterator(const base_node_type* p) noexcept: it_(const_cast<base_node_type*>(p)) {}

    list_iterator(const list_iterator&) noexcept = default;
    list_iterator(list_iterator&&) noexcept = default;

    template<class P, class R>
    list_iterator(const list_iterator<T, P, R>& other) noexcept : it_(const_cast<base_node_type*>(other.base())) {}

    ~list_iterator() = default;

    auto operator=(const list_iterator&) noexcept -> list_iterator& = default;
    auto operator=(list_iterator&&) noexcept -> list_iterator& = default;

    [[nodiscard]] auto next() const noexcept -> list_iterator {
        return list_iterator(it_->next_);
    }

    [[nodiscard]] auto prev() const noexcept -> list_iterator {
        return list_iterator(it_->prev_);
    }

    [[nodiscard]] auto operator*() const noexcept -> reference {
        return (static_cast<node_type*>(it_))->value_;
    }

    [[nodiscard]] auto operator->() const noexcept -> pointer {
        return &static_cast<node_type*>(it_)->value_;
    }

    auto operator++() noexcept -> list_iterator& {
        it_ = it_->next_;
        return *this;
    }

    [[nodiscard]] auto operator++(int) noexcept -> list_iterator {
        list_iterator res(it_);
        it_ = it_->next_;
        return res;
    }

    auto operator--() noexcept -> list_iterator& {
        it_ = it_->prev_;
        return *this;
    }

    [[nodiscard]] auto operator--(int) noexcept -> list_iterator {
        list_iterator res(it_);
        it_ = it_->prev_;
        return res;
    }

    [[nodiscard]] auto base() const noexcept -> base_node_type* {
        return it_;
    }

    [[nodiscard]] explicit operator bool() const noexcept {
        return it_ != nullptr;
    }

};    // class list_iterator

template<class T, class Pointer1, class Pointer2, class Reference1, class Reference2>
[[nodiscard]] auto operator==(const list_iterator<T, Pointer1, Reference1>& lhs,
                              const list_iterator<T, Pointer2, Reference2>& rhs) -> bool {
    return lhs.base() == rhs.base();
}

template<class T, class Pointer1, class Pointer2, class Reference1, class Reference2>
[[nodiscard]] auto operator!=(const list_iterator<T, Pointer1, Reference1>& lhs,
                              const list_iterator<T, Pointer2, Reference2>& rhs) -> bool {
    return !(lhs == rhs);
}

template<class T, class Allocator = allocator<T>>
class list {
public:
    using value_type             = T;
    using allocator_type         = Allocator;
    using size_type              = size_t;
    using difference_type        = ptrdiff_t;
    using reference              = value_type&;
    using const_reference        = const value_type&;

    using pointer                = typename allocator_traits<allocator_type>::pointer;
    using const_pointer          = typename allocator_traits<allocator_type>::const_pointer;

    using iterator               = list_iterator<value_type, pointer, reference>;
    using const_iterator         = list_iterator<value_type, const_pointer, const_reference>;

    using reverse_iterator       = ciel::reverse_iterator<iterator>;
    using const_reverse_iterator = ciel::reverse_iterator<const_iterator>;;

private:
    using base_node_type         = list_node_base;
    using node_type              = list_node<value_type>;

    using alloc_traits           = allocator_traits<allocator_type>;
    using node_allocator         = typename alloc_traits::template rebind_alloc<node_type>;
    using node_alloc_traits      = typename alloc_traits::template rebind_traits<node_type>;

    base_node_type end_node_;
    size_type size_;
    [[no_unique_address]] node_allocator allocator_;

    // end->prev_ points to before_begin_, before_begin_->next_ points to end
    auto alloc_range_destroy_and_deallocate(iterator begin, iterator end) noexcept -> iterator {
        size_ -= distance(begin, end);
        iterator loop = begin;
        iterator before_begin = begin.prev();
        while (loop != end) {
            auto* to_be_destroyed = static_cast<node_type*>(loop.base());
            ++loop;
            node_alloc_traits::destroy(allocator_, to_be_destroyed);
            node_alloc_traits::deallocate(allocator_, to_be_destroyed, 1);
        }
        before_begin.base()->next_ = end.base();
        end.base()->prev_ = before_begin.base();
        return end;
    }

    // We need to allocate for each node even if there is an N-size insertion
    template<class... Arg>
    auto alloc_range_allocate_and_construct_n(iterator begin, const size_type n, Arg&& ... arg) -> iterator {
        if (n == 0) {
            return begin;
        }
        iterator before_begin = begin.prev();
        iterator original_before_begin = before_begin;
        CIEL_TRY {
            for (size_type i = 0; i < n; ++i) {
                node_type *construct_place = node_alloc_traits::allocate(allocator_, 1);
                CIEL_TRY {
                    node_alloc_traits::construct(allocator_, construct_place, before_begin.base(), begin.base(),
                                                 std::forward<Arg>(arg)...);
                    ++size_;
                    before_begin.base()->next_ = construct_place;
                    begin.base()->prev_ = construct_place;
                    ++before_begin;
                } CIEL_CATCH (...) {
                    node_alloc_traits::deallocate(allocator_, construct_place, 1);
                    CIEL_THROW;
                }
            }
            return before_begin;
        } CIEL_CATCH (...) {
            alloc_range_destroy_and_deallocate(original_before_begin.next(), begin);
            CIEL_THROW;
        }
    }

    template<legacy_input_iterator Iter>
    auto alloc_range_allocate_and_construct(iterator begin, Iter first, Iter last) -> iterator {
        if (first == last) {
            return begin;
        }
        iterator before_begin = begin.prev();
        iterator original_before_begin = before_begin;
        CIEL_TRY {
            while (first != last) {
                node_type *construct_place = node_alloc_traits::allocate(allocator_, 1);
                CIEL_TRY {
                    node_alloc_traits::construct(allocator_, construct_place, before_begin.base(), begin.base(),
                                                 *first);
                    ++first;
                    ++size_;
                    before_begin.base()->next_ = construct_place;
                    begin.base()->prev_ = construct_place;
                    ++before_begin;
                } CIEL_CATCH (...) {
                    node_alloc_traits::deallocate(allocator_, construct_place, 1);
                    CIEL_THROW;
                }
            }
            return before_begin;
        } CIEL_CATCH (...) {
            alloc_range_destroy_and_deallocate(original_before_begin.next(), begin);
            CIEL_THROW;
        }
    }

public:
    list()
        : size_(0), allocator_() {}

    explicit list(const allocator_type& alloc)
        : size_(0), allocator_(alloc) {}

    list(const size_type count, const T& value, const allocator_type& alloc = allocator_type())
        : size_(0), allocator_(alloc) {
        alloc_range_allocate_and_construct_n(end(), count, value);
    }

    explicit list(const size_type count, const allocator_type& alloc = allocator_type())
        : size_(0), allocator_(alloc) {
        alloc_range_allocate_and_construct_n(end(), count);
    }

    template<legacy_input_iterator Iter>
    list(Iter first, Iter last, const allocator_type& alloc = allocator_type())
        : size_(0), allocator_(alloc) {
        alloc_range_allocate_and_construct(end(), first, last);
    }

    list(const list& other)
        : list(other.begin(), other.end(),
               alloc_traits::select_on_container_copy_construction(other.get_allocator())) {}

    list(const list& other, const allocator_type& alloc)
        : list(other.begin(), other.end(), alloc) {}

    list(list&& other) noexcept
        : end_node_(other.end_node_), size_(other.size_), allocator_(std::move(other.allocator_)) {
        end_node_.next_->prev_ = &end_node_;
        end_node_.prev_->next_ = &end_node_;
        other.end_node_.clear();
        other.size_ = 0;
    }

    list(list&& other, const allocator_type& alloc) {
        if (alloc == other.get_allocator()) {
            end_node_ = other.end_node_;
            size_ = other.size_;
            allocator_ = alloc;
            end_node_.next_->prev_ = &end_node_;
            end_node_.prev_->next_ = &end_node_;
            other.end_node_.clear();
            other.size_ = 0;
        } else {
            list(other, alloc).swap(*this);
        }
    }

    list(std::initializer_list<T> init, const allocator_type& alloc = allocator_type())
        : list(init.begin(), init.end(), alloc) {}

    ~list() {
        clear();
    }

    auto operator=(const list& other) -> list& {
        if (this == addressof(other)) {
            return *this;
        }
        if (alloc_traits::propagate_on_container_copy_assignment::value) {
            if (allocator_ != other.allocator_) {
                list(other.allocator_).swap(*this);
                alloc_range_allocate_and_construct(end(), other.begin(), other.end());
                return *this;
            }
            allocator_ = other.allocator_;
        }
        clear();
        alloc_range_allocate_and_construct(end(), other.begin(), other.end());
        return *this;
    }

    auto operator=(list&& other) noexcept(alloc_traits::is_always_equal::value) -> list& {
        if (this == addressof(other)) {
            return *this;
        }
        if (!alloc_traits::propagate_on_container_move_assignment::value && allocator_ != other.allocator_) {
            clear();
            alloc_range_allocate_and_construct(end(), other.begin(), other.end());
            return *this;
        }
        if (alloc_traits::propagate_on_container_move_assignment::value) {
            allocator_ = other.allocator_;
        }
        clear();
        end_node_ = other.end_node_;
        end_node_.next_->prev_ = &end_node_;
        end_node_.prev_->next_ = &end_node_;
        other.end_node_.clear();
        size_ = other.size_;
        other.size_ = 0;
        return *this;
    }

    auto operator=(std::initializer_list<T> ilist) -> list& {
        clear();
        alloc_range_allocate_and_construct(end(), ilist.begin(), ilist.end());
        return *this;
    }

    auto assign(size_type count, const T& value) -> void {
        clear();
        alloc_range_allocate_and_construct_n(end(), count, value);
    }

    template<legacy_input_iterator Iter>
    auto assign(Iter first, Iter last) -> void {
        clear();
        alloc_range_allocate_and_construct(end(), first, last);
    }

    auto assign(std::initializer_list<T> ilist) -> void {
        clear();
        alloc_range_allocate_and_construct(end(), ilist.begin(), ilist.end());
    }

    [[nodiscard]] auto get_allocator() const noexcept -> allocator_type {
        return allocator_;
    }

    [[nodiscard]] auto front() -> reference {
        return *begin();
    }

    [[nodiscard]] auto front() const -> const_reference {
        return *begin();
    }

    [[nodiscard]] auto back() -> reference {
        return *--end();
    }

    [[nodiscard]] auto back() const -> const_reference {
        return *--end();
    }

    [[nodiscard]] auto begin() noexcept -> iterator {
        return iterator(end_node_.next_);
    }

    [[nodiscard]] auto begin() const noexcept -> const_iterator {
        return const_iterator(end_node_.next_);
    }

    [[nodiscard]] auto cbegin() const noexcept -> const_iterator {
        return begin();
    }

    [[nodiscard]] auto end() noexcept -> iterator {
        return iterator(&end_node_);
    }

    [[nodiscard]] auto end() const noexcept -> const_iterator {
        return const_iterator(&end_node_);
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
        return numeric_limits<difference_type>::max();
    }

    auto clear() noexcept -> void {
        alloc_range_destroy_and_deallocate(begin(), end());
    }

    auto insert(const_iterator pos, const T& value) -> iterator {
        return alloc_range_allocate_and_construct_n(pos, 1, value);
    }

    auto insert(const_iterator pos, T&& value) -> iterator {
        return alloc_range_allocate_and_construct_n(pos, 1, std::move(value));
    }

    auto insert(const_iterator pos, const size_type count, const T& value) -> iterator {
        return alloc_range_allocate_and_construct_n(pos, count, value);
    }

    template<legacy_input_iterator Iter>
    auto insert(const_iterator pos, Iter first, Iter last) -> iterator {
        return alloc_range_allocate_and_construct(pos, first, last);
    }

    auto insert(const_iterator pos, std::initializer_list<T> ilist) -> iterator {
        return alloc_range_allocate_and_construct(pos, ilist.begin(), ilist.end());
    }

    template<class... Args>
    auto emplace(const_iterator pos, Args&& ... args) -> iterator {
        return alloc_range_allocate_and_construct_n(pos, 1, std::forward<Args>(args)...);
    }

    auto erase(const_iterator pos) -> iterator {
        return alloc_range_destroy_and_deallocate(pos, pos.next());
    }

    auto erase(const_iterator first, const_iterator last) -> iterator {
        return alloc_range_destroy_and_deallocate(first, last);
    }

    auto push_back(const T& value) -> void {
        emplace_back(value);
    }

    auto push_back(T&& value) -> void {
        emplace_back(std::move(value));
    }

    template<class... Args>
    auto emplace_back(Args&& ... args) -> reference {
        return *alloc_range_allocate_and_construct_n(end(), 1, std::forward<Args>(args)...);
    }

    auto pop_back() noexcept -> void {
        alloc_range_destroy_and_deallocate(end().prev(), end());
    }

    auto push_front(const T& value) -> void {
        emplace_front(value);
    }

    auto push_front(T&& value) -> void {
        emplace_front(std::move(value));
    }

    template<class... Args>
    auto emplace_front(Args&& ... args) -> reference {
        return *alloc_range_allocate_and_construct_n(begin(), 1, std::forward<Args>(args)...);
    }

    auto pop_front() noexcept -> void {
        alloc_range_destroy_and_deallocate(begin(), begin().next());
    }

    auto resize(const size_type count) -> void {
        if (size() >= count) {
            iterator tmp = prev(end(), size() - count);
            alloc_range_destroy_and_deallocate(tmp, end());
        } else {
            alloc_range_allocate_and_construct_n(end(), count - size());
        }
    }

    auto resize(const size_type count, const value_type& value) -> void {
        if (size() >= count) {
            iterator tmp = prev(end(), size() - count);
            alloc_range_destroy_and_deallocate(tmp, end());
        } else {
            alloc_range_allocate_and_construct_n(end(), count - size(), value);
        }
    }

    auto swap(list& other) noexcept(alloc_traits::is_always_equal::value) -> void {
        using std::swap;

        swap(end_node_, other.end_node_);
        end_node_.next_->prev_ = &end_node_;
        end_node_.prev_->next_ = &end_node_;
        other.end_node_.next_->prev_ = &other.end_node_;
        other.end_node_.prev_->next_ = &other.end_node_;
        swap(size_, other.size_);
        swap(allocator_, other.allocator_);
    }

};    // class list

template<class T, class Alloc>
[[nodiscard]] auto operator==(const list<T, Alloc>& lhs, const list<T, Alloc>& rhs) -> bool {
    if (lhs.size() != rhs.size()) {
        return false;
    }
    return equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<class T, class Alloc, class U>
auto erase(list<T, Alloc>& c, const U& value) -> typename list<T, Alloc>::size_type {
    return c.remove_if([&](auto& elem) { return elem == value; });
}

template<class T, class Alloc, class Pred>
auto erase_if(list<T, Alloc>& c, Pred pred) -> typename list<T, Alloc>::size_type {
    return c.remove_if(pred);
}

template<legacy_input_iterator Iter, class Alloc = allocator<typename iterator_traits<Iter>::value_type>>
list(Iter, Iter, Alloc = Alloc()) -> list<typename iterator_traits<Iter>::value_type, Alloc>;

NAMESPACE_CIEL_END

namespace std {

template<class T, class Alloc>
auto swap(ciel::list<T, Alloc>& lhs, ciel::list<T, Alloc>& rhs) noexcept(noexcept(lhs.swap(rhs))) -> void {
    lhs.swap(rhs);
}

}   // namespace std

#endif // CIELUTILS_INCLUDE_CIEL_LIST_HPP_