#ifndef CIELUTILS_INCLUDE_CIEL_SET_HPP_
#define CIELUTILS_INCLUDE_CIEL_SET_HPP_

#include <ciel/avl.hpp>
#include <ciel/config.hpp>
#include <ciel/memory_impl/allocator.hpp>

NAMESPACE_CIEL_BEGIN

// TODO: operator<=>, node_type, insert_return_type, extract, merge

template<class Key, class Compare = less<Key>, class Allocator = allocator<Key>>
class set {
public:
    using key_type               = Key;
    using value_type             = Key;
    using size_type              = size_t;
    using difference_type        = ptrdiff_t;
    using key_compare            = Compare;
    using value_compare          = Compare;
    using allocator_type         = Allocator;
    using reference              = value_type&;
    using const_reference        = const value_type&;
    using pointer                = typename allocator_traits<allocator_type>::pointer;
    using const_pointer          = typename allocator_traits<allocator_type>::const_pointer;

private:
    using tree_type              = avl<value_type, value_compare, allocator_type>;
    using alloc_traits           = allocator_traits<allocator_type>;

public:
    using iterator               = typename tree_type::iterator;  // FIXME: set is not supposed to be modified
    using const_iterator         = typename tree_type::const_iterator;

    using reverse_iterator       = reverse_iterator<iterator>;
    using const_reverse_iterator = ciel::reverse_iterator<const_iterator>;

private:
    tree_type tree_;

public:
    set() : tree_() {}

    explicit set(const value_compare& c, const allocator_type& alloc = allocator_type()) : tree_(c, alloc) {}

    explicit set(const allocator_type& alloc) : tree_(alloc) {}

    template<legacy_input_iterator Iter>
    set(Iter first, Iter last, const value_compare& c = Compare(),
        const allocator_type& alloc = allocator_type()) : tree_(false_type{}, first, last, c, alloc) {}

    template<legacy_input_iterator Iter>
    set(Iter first, Iter last, const allocator_type& alloc): set(first, last, value_compare(), alloc) {}

    set(const set& other) : tree_(other.tree_) {}

    set(const set& other, const allocator_type& alloc) : tree_(other.tree_, alloc) {}

    set(set&& other) noexcept : tree_(std::move(other.tree_)) {}

    set(set&& other, const allocator_type& alloc) : tree_(std::move(other.tree_), alloc) {}

    set(std::initializer_list<value_type> init, const value_compare& c = value_compare(),
        const allocator_type& alloc = allocator_type()) : set(init.begin(), init.end(), c, alloc) {}

    set(std::initializer_list<value_type> init, const allocator_type& alloc) : set(init, value_compare(), alloc) {}

    ~set() = default;

    auto operator=(const set& other) -> set& = default;

    auto operator=(set&& other)
        noexcept(alloc_traits::is_always_equal::value && is_nothrow_move_assignable_v<value_compare>) -> set& = default;

    auto operator=(std::initializer_list<value_type> ilist) -> set& {
        clear();
        tree_.range_insert_unique(ilist.begin(), ilist.end());
        return *this;
    }

    [[nodiscard]] auto get_allocator() const noexcept -> allocator_type {
        return tree_.get_allocator();
    }

    [[nodiscard]] auto begin() noexcept -> iterator {
        return tree_.begin();
    }

    [[nodiscard]] auto begin() const noexcept -> const_iterator {
        return tree_.begin();
    }

    [[nodiscard]] auto cbegin() const noexcept -> const_iterator {
        return begin();
    }

    [[nodiscard]] auto end() noexcept -> iterator {
        return tree_.end();
    }

    [[nodiscard]] auto end() const noexcept -> const_iterator {
        return tree_.end();
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
        return tree_.empty();
    }

    [[nodiscard]] auto size() const noexcept -> size_type {
        return tree_.size();
    }

    [[nodiscard]] auto max_size() const noexcept -> size_type {
        return tree_.max_size();
    }

    auto clear() noexcept -> void {
        tree_.clear();
    }

    auto insert(const value_type& value) -> pair<iterator, bool> {
        return emplace(value);
    }

    auto insert(value_type&& value) -> pair<iterator, bool> {
        return emplace(std::move(value));
    }

    auto insert(const_iterator hint, const value_type& value) -> iterator {
        return emplace_hint(hint, value);
    }

    auto insert(const_iterator hint, value_type&& value) -> iterator {
        return emplace_hint(hint, std::move(value));
    }

    template<legacy_input_iterator Iter>
    auto insert(Iter first, Iter last) -> void {
        tree_.range_insert_unique(first, last);
    }

    auto insert(std::initializer_list<value_type> ilist) -> void {
        insert(ilist.begin(), ilist.end());
    }

    template<class... Args>
    auto emplace(Args&& ... args) -> pair<iterator, bool> {
        return tree_.emplace_unique(std::forward<Args>(args)...);
    }

    template<class... Args>
    auto emplace_hint(const_iterator hint, Args&& ... args) -> iterator {
        return tree_.emplace_unique_hint(hint, std::forward<Args>(args)...);
    }

    auto erase(iterator pos) -> iterator {
        return erase(pos, pos.next());
    }

    auto erase(const_iterator pos) -> iterator {
        return erase(pos, pos.next());
    }

    auto erase(const_iterator first, const_iterator last) -> iterator {
        return tree_.erase(first, last);
    }

    auto erase(const Key& key) -> size_type {
        return tree_.erase(key);
    }

    template<class K>
        requires requires { typename value_compare::is_transparent; }
    auto erase(K&& x) -> size_type {
        return tree_.erase(x);
    }

    auto swap(set& other)
        noexcept(alloc_traits::is_always_equal::value && is_nothrow_swappable_v<value_compare>) -> void {
        tree_.swap(other.tree_);
    }

    [[nodiscard]] auto count(const Key& key) const -> size_type {
        return tree_.count_unique(key);
    }

    template<class K>
        requires requires { typename value_compare::is_transparent; }
    [[nodiscard]] auto count(const K& x) const -> size_type {
        return tree_.count_unique(x);
    }

    [[nodiscard]] auto find(const Key& key) -> iterator {
        return tree_.find(key);
    }

    [[nodiscard]] auto find(const Key& key) const -> const_iterator {
        return tree_.find(key);
    }

    template<class K>
        requires requires { typename value_compare::is_transparent; }
    [[nodiscard]] auto find(const K& x) -> iterator {
        return tree_.find(x);
    }

    template<class K>
        requires requires { typename value_compare::is_transparent; }
    [[nodiscard]] auto find(const K& x) const -> const_iterator {
        return tree_.find(x);
    }

    [[nodiscard]] auto contains(const Key& key) const -> bool {
        return tree_.contains(key);
    }

    template<class K>
        requires requires { typename value_compare::is_transparent; }
    [[nodiscard]] auto contains(const K& x) const -> bool {
        return tree_.contains(x);
    }

    [[nodiscard]] auto equal_range(const Key& key) -> pair<iterator, iterator> {
        return tree_.equal_range(key);
    }

    [[nodiscard]] auto equal_range(const Key& key) const -> pair<const_iterator, const_iterator> {
        return tree_.equal_range(key);
    }

    template<class K>
        requires requires { typename value_compare::is_transparent; }
    [[nodiscard]] auto equal_range(const K& x) -> pair<iterator, iterator> {
        return tree_.equal_range(x);
    }

    template<class K>
        requires requires { typename value_compare::is_transparent; }
    [[nodiscard]] auto equal_range(const K& x) const -> pair<const_iterator, const_iterator> {
        return tree_.equal_range(x);
    }

    [[nodiscard]] auto lower_bound(const Key& key) -> iterator {
        return tree_.lower_bound(key);
    }

    [[nodiscard]] auto lower_bound(const Key& key) const -> const_iterator {
        return tree_.lower_bound(key);
    }

    template<class K>
        requires requires { typename value_compare::is_transparent; }
    [[nodiscard]] auto lower_bound(const K& x) -> iterator {
        return tree_.lower_bound(x);
    }

    template<class K>
        requires requires { typename value_compare::is_transparent; }
    [[nodiscard]] auto lower_bound(const K& x) const -> const_iterator {
        return tree_.lower_bound(x);
    }

    [[nodiscard]] auto upper_bound(const Key& key) -> iterator {
        return tree_.upper_bound(key);
    }

    [[nodiscard]] auto upper_bound(const Key& key) const -> const_iterator {
        return tree_.upper_bound(key);
    }

    template<class K>
        requires requires { typename value_compare::is_transparent; }
    [[nodiscard]] auto upper_bound(const K& x) -> iterator {
        return tree_.upper_bound(x);
    }

    template<class K>
        requires requires { typename value_compare::is_transparent; }
    [[nodiscard]] auto upper_bound(const K& x) const -> const_iterator {
        return tree_.upper_bound(x);
    }

    [[nodiscard]] auto key_comp() const -> key_compare {
        return value_comp();
    }

    [[nodiscard]] auto value_comp() const -> value_compare {
        return tree_.value_comp();
    }

};  // class set

template<class Key, class Compare, class Alloc>
[[nodiscard]] auto operator==(const set<Key, Compare, Alloc>& lhs, const set<Key, Compare, Alloc>& rhs) -> bool {
    if (lhs.size() != rhs.size()) {
        return false;
    }
    return equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<class Key, class Compare, class Alloc, class Pred>
auto erase_if(set<Key, Compare, Alloc>& c, Pred pred) -> typename set<Key, Compare, Alloc>::size_type {
    auto old_size = c.size();
    for (auto i = c.begin(), last = c.end(); i != last;) {
        if (pred(*i)) {
            i = c.erase(i);
        } else {
            ++i;
        }
    }
    return old_size - c.size();
}

template<legacy_input_iterator Iter,
    class Comp = less<typename iterator_traits<Iter>::value_type>,
        class Alloc = allocator<typename iterator_traits<Iter>::value_type>>
set(Iter, Iter, Comp = Comp(), Alloc = Alloc()) -> set<typename iterator_traits<Iter>::value_type, Comp, Alloc>;

template<class Key, class Comp = less<Key>, class Alloc = allocator<Key>>
set(std::initializer_list<Key>, Comp = Comp(), Alloc = Alloc()) -> set<Key, Comp, Alloc>;

template<legacy_input_iterator Iter, class Alloc>
set(Iter, Iter, Alloc)
    -> set<typename iterator_traits<Iter>::value_type, less<typename iterator_traits<Iter>::value_type>, Alloc>;

template<class Key, class Alloc>
set(std::initializer_list<Key>, Alloc) -> set<Key, less<Key>, Alloc>;

NAMESPACE_CIEL_END

namespace std {

template<class Key, class Compare, class Alloc>
auto swap(ciel::set<Key, Compare, Alloc>& lhs,
          ciel::set<Key, Compare, Alloc>& rhs) noexcept(noexcept(lhs.swap(rhs))) -> void {
    lhs.swap(rhs);
}

}   // namespace std

#endif // CIELUTILS_INCLUDE_CIEL_SET_HPP_