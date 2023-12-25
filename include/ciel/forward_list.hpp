#ifndef CIELUTILS_INCLUDE_CIEL_FORWARD_LIST_HPP_
#define CIELUTILS_INCLUDE_CIEL_FORWARD_LIST_HPP_

#include <ciel/config.hpp>
#include <ciel/iterator_impl/iterator_traits.hpp>
#include <ciel/iterator_impl/legacy_input_iterator.hpp>
#include <ciel/iterator_impl/next.hpp>
#include <ciel/memory_impl/allocator.hpp>
#include <ciel/memory_impl/allocator_traits.hpp>
#include <cstddef>

NAMESPACE_CIEL_BEGIN

// TODO: operations like merge and sort

// To keep the struct lightweight, we don't have a size() in forward_list.
// Different from list, forward_list is not a circle, which makes end() == iterator(nullptr)

struct forward_list_node_base {
    forward_list_node_base* next_;

    explicit forward_list_node_base(forward_list_node_base* n = nullptr) noexcept : next_(n) {}

};    // struct forward_list_node_base

template<class T>
struct forward_list_node : forward_list_node_base {
    T value_;

    template<class... Args>
    explicit forward_list_node(forward_list_node_base* n, Args&& ... args)
        : forward_list_node_base(n), value_(std::forward<Args>(args)...) {}

};    // struct forward_list_node

template<class T, class Pointer, class Reference>
class forward_list_iterator {
public:
    using difference_type   = ptrdiff_t;
    using value_type        = T;
    using pointer           = Pointer;
    using reference         = Reference;
    using iterator_category = forward_iterator_tag;
    using iterator_concept  = forward_iterator_tag;

private:
    using base_node_type    = forward_list_node_base;
    using node_type         = forward_list_node<value_type>;

    base_node_type* it_;

public:
    forward_list_iterator() noexcept : it_(nullptr) {}

    explicit forward_list_iterator(const base_node_type* p) noexcept : it_(const_cast<base_node_type*>(p)) {}

    forward_list_iterator(const forward_list_iterator&) noexcept = default;
    forward_list_iterator(forward_list_iterator&&) noexcept = default;

    template<class P, class R>
    forward_list_iterator(const forward_list_iterator<T, P, R>& other) noexcept
        : it_(const_cast<base_node_type*>(other.base())) {}

    ~forward_list_iterator() = default;

    auto operator=(const forward_list_iterator&) noexcept -> forward_list_iterator& = default;
    auto operator=(forward_list_iterator&&) noexcept -> forward_list_iterator& = default;

    [[nodiscard]] auto next() const noexcept -> forward_list_iterator {
        return forward_list_iterator(it_->next_);
    }

    [[nodiscard]] auto operator*() const noexcept -> reference {
        return static_cast<node_type*>(it_)->value_;
    }

    [[nodiscard]] auto operator->() const noexcept -> pointer {
        return &static_cast<node_type*>(it_)->value_;
    }

    auto operator++() noexcept -> forward_list_iterator& {
        it_ = it_->next_;
        return *this;
    }

    [[nodiscard]] auto operator++(int) noexcept -> forward_list_iterator {
        forward_list_iterator res(it_);
        ++(*this);
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
[[nodiscard]] auto operator==(const forward_list_iterator<T, Pointer1, Reference1>& lhs,
                              const forward_list_iterator<T, Pointer2, Reference2>& rhs) -> bool {
    return lhs.base() == rhs.base();
}

template<class T, class Pointer1, class Pointer2, class Reference1, class Reference2>
[[nodiscard]] auto operator!=(const forward_list_iterator<T, Pointer1, Reference1>& lhs,
                              const forward_list_iterator<T, Pointer2, Reference2>& rhs) -> bool {
    return !(lhs == rhs);
}

template<class T, class Allocator = allocator<T>>
class forward_list {
public:
    using value_type        = T;
    using allocator_type    = Allocator;
    using size_type         = size_t;
    using difference_type   = ptrdiff_t;
    using reference         = value_type&;
    using const_reference   = const value_type&;

    using pointer           = typename allocator_traits<allocator_type>::pointer;
    using const_pointer     = typename allocator_traits<allocator_type>::const_pointer;

    using iterator          = forward_list_iterator<value_type, pointer, reference>;
    using const_iterator    = forward_list_iterator<value_type, const_pointer, const_reference>;

private:
    using base_node_type    = forward_list_node_base;
    using node_type         = forward_list_node<value_type>;

    using alloc_traits      = allocator_traits<allocator_type>;
    using node_allocator    = typename alloc_traits::template rebind_alloc<node_type>;
    using node_alloc_traits = typename alloc_traits::template rebind_traits<node_type>;

    base_node_type before_begin_;
    [[no_unique_address]] node_allocator allocator_;

    // Note that this range is (begin, end)
    auto alloc_range_destroy_and_deallocate(iterator begin, iterator end) noexcept -> iterator {
        CIEL_PRECONDITION(begin != end);

        iterator loop = begin.next();
        while (loop != end) {
            auto* to_be_destroyed = static_cast<node_type*>(loop.base());
            ++loop;
            node_alloc_traits::destroy(allocator_, to_be_destroyed);
            node_alloc_traits::deallocate(allocator_, to_be_destroyed, 1);
        }
        begin.base()->next_ = end.base();
        return end;
    }

    // We need to allocate for each node even if there is an N-size insertion
    template<class... Arg>
    auto alloc_range_allocate_and_construct_n(iterator begin, const size_type n, Arg&& ... arg) -> iterator {
        // We insert between this two, so when it throws we can destroy and deallocate previous inserted nodes
        iterator original_begin = begin;
        iterator after_begin = begin.next();

        CIEL_TRY {
            for (size_type i = 0; i < n; ++i) {
                node_type* construct_place = node_alloc_traits::allocate(allocator_, 1);
                CIEL_TRY {
                    node_alloc_traits::construct(allocator_, construct_place, after_begin.base(),
                                                 std::forward<Arg>(arg)...);
                    begin.base()->next_ = construct_place;
                    ++begin;

                } CIEL_CATCH (...) {
                    node_alloc_traits::deallocate(allocator_, construct_place, 1);
                    CIEL_THROW;
                }
            }
            return begin;

        } CIEL_CATCH (...) {
            alloc_range_destroy_and_deallocate(original_begin, after_begin);
            CIEL_THROW;
        }
    }

    template<legacy_input_iterator Iter>
    auto alloc_range_allocate_and_construct(iterator begin, Iter first, Iter last) -> iterator {
        // We insert between this two, so when it throws we can destroy and deallocate previous inserted nodes
        iterator original_begin = begin;
        iterator after_begin = begin.next();

        CIEL_TRY {
            while (first != last) {
                node_type *construct_place = node_alloc_traits::allocate(allocator_, 1);
                CIEL_TRY {
                    node_alloc_traits::construct(allocator_, construct_place, after_begin.base(), *first);
                    ++first;
                    begin.base()->next_ = construct_place;
                    ++begin;

                } CIEL_CATCH (...) {
                    node_alloc_traits::deallocate(allocator_, construct_place, 1);
                    CIEL_THROW;
                }
            }
            return begin;

        } CIEL_CATCH (...) {
            alloc_range_destroy_and_deallocate(original_begin, after_begin);
            CIEL_THROW;
        }
    }

public:
    forward_list()
        : allocator_() {}

    explicit forward_list(const allocator_type& alloc)
        : allocator_(alloc) {}

    forward_list(const size_type count, const T& value, const allocator_type& alloc = allocator_type())
        : allocator_(alloc) {
        alloc_range_allocate_and_construct_n(before_begin(), count, value);
    }

    explicit forward_list(const size_type count, const allocator_type& alloc = allocator_type())
        : allocator_(alloc) {
        alloc_range_allocate_and_construct_n(before_begin(), count);
    }

    template<legacy_input_iterator Iter>
    forward_list(Iter first, Iter last, const allocator_type& alloc = allocator_type())
        : allocator_(alloc) {
        alloc_range_allocate_and_construct(before_begin(), first, last);
    }

    forward_list(const forward_list& other)
        : forward_list(other.begin(), other.end(),
                       alloc_traits::select_on_container_copy_construction(other.get_allocator())) {}

    forward_list(const forward_list& other, const allocator_type& alloc)
        : forward_list(other.begin(), other.end(), alloc) {}

    forward_list(forward_list&& other) noexcept
        : before_begin_(other.before_begin_), allocator_(std::move(other.allocator_)) {
        other.before_begin_.next_ = nullptr;
    }

    forward_list(forward_list&& other, const allocator_type& alloc) {
        if (alloc == other.get_allocator()) {
            allocator_ = alloc;
            before_begin_ = other.before_begin_;
            other.before_begin_.next_ = nullptr;
        } else {
            forward_list(other, alloc).swap(*this);
        }
    }

    forward_list(std::initializer_list<T> init, const allocator_type& alloc = allocator_type())
        : forward_list(init.begin(), init.end(), alloc) {}

    ~forward_list() {
        clear();
    }

    auto operator=(const forward_list& other) -> forward_list& {
        if (this == addressof(other)) {
            return *this;
        }
        if (alloc_traits::propagate_on_container_copy_assignment::value) {
            if (allocator_ != other.allocator_) {
                forward_list(other.allocator_).swap(*this);
                alloc_range_allocate_and_construct(before_begin(), other.begin(), other.end());
                return *this;
            }
            allocator_ = other.allocator_;
        }
        clear();
        alloc_range_allocate_and_construct(before_begin(), other.begin(), other.end());
        return *this;
    }

    auto operator=(forward_list&& other) noexcept(alloc_traits::is_always_equal::value) -> forward_list& {
        if (this == addressof(other)) {
            return *this;
        }
        if (!alloc_traits::propagate_on_container_move_assignment::value && allocator_ != other.allocator_) {
            clear();
            alloc_range_allocate_and_construct(before_begin(), other.begin(), other.end());
            return *this;
        }
        if (alloc_traits::propagate_on_container_move_assignment::value) {
            allocator_ = std::move(other.allocator_);
        }
        clear();
        before_begin_ = other.before_begin_;
        other.before_begin_.next_ = nullptr;
        return *this;
    }

    auto operator=(std::initializer_list<T> ilist) -> forward_list& {
        clear();
        alloc_range_allocate_and_construct(before_begin(), ilist.begin(), ilist.end());
        return *this;
    }

    auto assign(const size_type count, const T& value) -> void {
        clear();
        alloc_range_allocate_and_construct_n(before_begin(), count, value);
    }

    template<legacy_input_iterator Iter>
    auto assign(Iter first, Iter last) -> void {
        clear();
        alloc_range_allocate_and_construct(before_begin(), first, last);
    }

    auto assign(std::initializer_list<T> ilist) -> void {
        clear();
        alloc_range_allocate_and_construct(before_begin(), ilist.begin(), ilist.end());
    }

    [[nodiscard]] auto get_allocator() const noexcept -> allocator_type {
        return allocator_;
    }

    [[nodiscard]] auto front() -> reference {
        CIEL_PRECONDITION(!empty());

        return *begin();
    }

    [[nodiscard]] auto front() const -> const_reference {
        CIEL_PRECONDITION(!empty());

        return *begin();
    }

    [[nodiscard]] auto before_begin() noexcept -> iterator {
        return iterator(&before_begin_);
    }

    [[nodiscard]] auto before_begin() const noexcept -> const_iterator {
        return const_iterator(&before_begin_);
    }

    [[nodiscard]] auto cbefore_begin() const noexcept -> const_iterator {
        return before_begin();
    }

    [[nodiscard]] auto begin() noexcept -> iterator {
        return iterator(before_begin_.next_);
    }

    [[nodiscard]] auto begin() const noexcept -> const_iterator {
        return const_iterator(before_begin_.next_);
    }

    [[nodiscard]] auto cbegin() const noexcept -> const_iterator {
        return begin();
    }

    [[nodiscard]] auto end() noexcept -> iterator {
        return iterator(nullptr);
    }

    [[nodiscard]] auto end() const noexcept -> const_iterator {
        return const_iterator(nullptr);
    }

    [[nodiscard]] auto cend() const noexcept -> const_iterator {
        return end();
    }

    [[nodiscard]] auto empty() const noexcept -> bool {
        return begin() == end();
    }

    [[nodiscard]] auto size() const noexcept -> size_type {
        size_type res = 0;
        iterator loop = before_begin();
        while (loop.next()) {
            ++loop;
            ++res;
        }
        return res;
    }

    [[nodiscard]] auto max_size() const noexcept -> size_type {
        return numeric_limits<difference_type>::max();
    }

    auto clear() noexcept -> void {
        alloc_range_destroy_and_deallocate(before_begin(), end());
    }

    auto insert_after(iterator pos, const T& value) -> iterator {
        return alloc_range_allocate_and_construct_n(pos, 1, value);
    }

    auto insert_after(iterator pos, T&& value) -> iterator {
        return alloc_range_allocate_and_construct_n(pos, 1, std::move(value));
    }

    auto insert_after(iterator pos, const size_type count, const T& value) -> iterator {
        return alloc_range_allocate_and_construct_n(pos, count, value);
    }

    template<legacy_input_iterator Iter>
    auto insert_after(iterator pos, Iter first, Iter last) -> iterator {
        return alloc_range_allocate_and_construct(pos, first, last);
    }

    auto insert_after(iterator pos, std::initializer_list<T> ilist) -> iterator {
        return alloc_range_allocate_and_construct(pos, ilist.begin(), ilist.end());
    }

    template<class... Args>
    auto emplace_after(iterator pos, Args&& ... args) -> iterator {
        return alloc_range_allocate_and_construct_n(pos, 1, std::forward<Args>(args)...);
    }

    auto erase_after(iterator pos) -> iterator {
        if (pos.next() == end()) {
            return end();
        }
        return alloc_range_destroy_and_deallocate(pos, pos.next().next());
    }

    auto erase_after(iterator first, iterator last) -> iterator {
        return alloc_range_destroy_and_deallocate(first, last);
    }

    auto push_front(const T& value) -> void {
        emplace_front(value);
    }

    auto push_front(T&& value) -> void {
        emplace_front(std::move(value));
    }

    template<class... Args>
    auto emplace_front(Args&& ... args) -> reference {
        return *alloc_range_allocate_and_construct_n(before_begin(), 1, std::forward<Args>(args)...);
    }

    auto pop_front() noexcept -> void {
        CIEL_PRECONDITION(!empty());

        alloc_range_destroy_and_deallocate(before_begin(), begin().next());
    }

    auto resize(const size_type count) -> void {
        if (const size_type s = size(); s >= count) {
            iterator tmp = next(before_begin(), count);
            alloc_range_destroy_and_deallocate(tmp, end());
        } else {
            iterator tmp = next(before_begin(), s);
            alloc_range_allocate_and_construct_n(tmp, count - s);
        }
    }

    auto resize(const size_type count, const value_type& value) -> void {
        if (const size_type s = size(); s >= count) {
            iterator tmp = next(before_begin(), count);
            alloc_range_destroy_and_deallocate(tmp, end());
        } else {
            iterator tmp = next(before_begin(), s);
            alloc_range_allocate_and_construct_n(tmp, count - s, value);
        }
    }

    auto swap(forward_list& other) noexcept(alloc_traits::is_always_equal::value) -> void {
        using std::swap;
        swap(before_begin_, other.before_begin_);
        swap(allocator_, other.allocator_);
    }

};    // class forward_list

template<class T, class Alloc>
[[nodiscard]] auto operator==(const forward_list<T, Alloc>& lhs, const forward_list<T, Alloc>& rhs) -> bool {
    auto l = lhs.begin();
    auto r = rhs.begin();
    while (l != lhs.end() && r != rhs.end()) {
        if (*l == *r) {
            ++l;
            ++r;
        } else {
            return false;
        }
    }
    return l == lhs.end() && r == rhs.end();
}

template<class T, class Alloc, class U>
auto erase(forward_list<T, Alloc>& c, const U& value) -> typename forward_list<T, Alloc>::size_type {
    return c.remove_if([&](auto&& elem) { return elem == value; });
}

template<class T, class Alloc, class Pred>
auto erase_if(forward_list<T, Alloc>& c, Pred pred) -> typename forward_list<T, Alloc>::size_type {
    return c.remove_if(pred);
}

template<legacy_input_iterator Iter, class Alloc = allocator<typename iterator_traits<Iter>::value_type>>
forward_list(Iter, Iter, Alloc = Alloc()) -> forward_list<typename iterator_traits<Iter>::value_type, Alloc>;

NAMESPACE_CIEL_END

namespace std {

template<class T, class Alloc>
auto swap(ciel::forward_list<T, Alloc>& lhs,
          ciel::forward_list<T, Alloc>& rhs) noexcept(noexcept(lhs.swap(rhs))) -> void {
    lhs.swap(rhs);
}

}   // namespace std

#endif // CIELUTILS_INCLUDE_CIEL_FORWARD_LIST_HPP_