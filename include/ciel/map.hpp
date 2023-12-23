#ifndef CIELUTILS_INCLUDE_CIEL_MAP_HPP_
#define CIELUTILS_INCLUDE_CIEL_MAP_HPP_

#include <ciel/avl.hpp>
#include <ciel/config.hpp>
#include <ciel/memory_impl/allocator.hpp>
#include <ciel/tuple.hpp>
#include <stdexcept>

NAMESPACE_CIEL_BEGIN

// TODO: operator<=>, node_type, insert_return_type, extract, merge

template<class Key, class T, class Compare = less<Key>, class Allocator = allocator<pair<const Key, T>>>
class map {
public:
    using key_type        = Key;
    using mapped_type     = T;
    using value_type      = pair<const Key, T>;
    using size_type       = size_t;
    using difference_type = ptrdiff_t;
    using key_compare     = Compare;
    using allocator_type  = Allocator;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using pointer         = typename allocator_traits<allocator_type>::pointer;
    using const_pointer   = typename allocator_traits<allocator_type>::const_pointer;

private:
    class value_compare {
    private:
        Compare comp_;

    public:
        explicit value_compare(const Compare& c = Compare()) : comp_(c) {}

        [[nodiscard]] auto operator()(const value_type& lhs, const value_type& rhs) const -> bool {
            return comp_(lhs.first, rhs.first);
        }

        [[nodiscard]] auto operator()(const key_type& lhs, const value_type& rhs) const -> bool {
            return comp_(lhs, rhs.first);
        }

        [[nodiscard]] auto operator()(const value_type& lhs, const key_type& rhs) const -> bool {
            return comp_(lhs.first, rhs);
        }

    };  // class value_compare

    using tree_type              = avl<value_type, value_compare, allocator_type>;
    using alloc_traits           = allocator_traits<allocator_type>;

public:
    using iterator               = typename tree_type::iterator;
    using const_iterator         = typename tree_type::const_iterator;

    using reverse_iterator       = reverse_iterator<iterator>;
    using const_reverse_iterator = ciel::reverse_iterator<const_iterator>;

private:
    tree_type tree_;

public:
    map() : tree_() {}

    explicit map(const value_compare& c, const allocator_type& alloc = allocator_type()) : tree_(c, alloc) {}

    explicit map(const Allocator& alloc) : tree_(alloc) {}

    template<legacy_input_iterator Iter>
    map(Iter first, Iter last, const value_compare& c = value_compare(), const allocator_type& alloc = allocator_type())
        : tree_(false_type{}, first, last, c, alloc) {}

    template<legacy_input_iterator Iter>
    map(Iter first, Iter last, const allocator_type& alloc) : map(first, last, value_compare(), alloc) {}

    map(const map& other) : tree_(other.tree_) {}

    map(const map& other, const allocator_type& alloc) : tree_(other.tree_, alloc) {}

    map(map&& other) noexcept : tree_(std::move(other.tree_)) {}

    map(map&& other, const allocator_type& alloc) : tree_(std::move(other.tree_), alloc) {}

    map(std::initializer_list<value_type> init,
        const value_compare& c = value_compare(),
        const allocator_type& alloc = allocator_type())
        : map(init.begin(), init.end(), c, alloc) {}

    map(std::initializer_list<value_type> init, const allocator_type& alloc) : map(init, value_compare(), alloc) {}

    ~map() = default;

    auto operator=(const map& other) -> map& = default;

    auto operator=(map&& other)
        noexcept(alloc_traits::is_always_equal::value && is_nothrow_move_assignable_v<value_compare>) -> map& = default;

    auto operator=(std::initializer_list<value_type> ilist) -> map& {
        clear();
        tree_.range_insert_unique(ilist.begin(), ilist.end());
        return *this;
    }

    [[nodiscard]] auto get_allocator() const noexcept -> allocator_type {
        return tree_.get_allocator();
    }

    [[nodiscard]] auto at(const Key& key) -> T& {
        auto pos = tree_.find(key);
        if (pos == end()) {
            THROW(std::out_of_range("key is not found in ciel::map"));
        }
        return pos->second;
    }

    [[nodiscard]] auto at(const Key& key) const -> const T& {
        auto pos = tree_.find(key);
        if (pos == end()) {
            THROW(std::out_of_range("key is not found in ciel::map"));
        }
        return pos->second;
    }

    [[nodiscard]] auto operator[](const Key& key) -> T& {
        return try_emplace(key).first->second;
    }

    [[nodiscard]] auto operator[](Key&& key) -> T& {
        return try_emplace(std::move(key)).first->second;
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

    template<class P>
        requires is_constructible_v<value_type, P&&>
    auto insert(P&& value) -> pair<iterator, bool> {
        return emplace(std::move(value));
    }

    auto insert(value_type&& value) -> pair<iterator, bool> {
        return emplace(std::move(value));
    }

    auto insert(const_iterator pos, const value_type& value) -> iterator {
        return emplace_hint(pos, value);
    }

    template<class P>
        requires is_constructible_v<value_type, P&&>
    auto insert(const_iterator pos, P&& value) -> iterator {
        return emplace_hint(pos, std::move(value));
    }

    auto insert(const_iterator pos, value_type&& value) -> iterator {
        return emplace_hint(pos, std::move(value));
    }

    template<legacy_input_iterator Iter>
    auto insert(Iter first, Iter last) -> void {
        tree_.range_insert_unique(first, last);
    }

    auto insert(std::initializer_list<value_type> ilist) -> void {
        insert(ilist.begin(), ilist.end());
    }

    template<class M>
        requires is_assignable_v<mapped_type&, M&&>
    auto insert_or_assign(const key_type& k, M&& obj) -> pair<iterator, bool> {
        if (auto pos = find(k); pos == end()) {
            return emplace(k, std::move(obj));
        } else {
            pos->second = std::move(obj);
            return {pos, false};
        }
    }

    template<class M>
        requires is_assignable_v<mapped_type&, M&&>
    auto insert_or_assign(key_type&& k, M&& obj) -> pair<iterator, bool> {
        if (auto pos = find(k); pos == end()) {
            return emplace(std::move(k), std::move(obj));
        } else {
            pos->second = std::move(obj);
            return {pos, false};
        }
    }

    template<class M>
        requires is_assignable_v<mapped_type&, M&&>
    auto insert_or_assign(const_iterator hint, const key_type& k, M&& obj) -> iterator {
        if (auto pos = find(k); pos == end()) {
            return emplace_hint(hint, k, std::move(obj));
        } else {
            pos->second = std::move(obj);
            return pos;
        }
    }

    template<class M>
        requires is_assignable_v<mapped_type&, M&&>
    auto insert_or_assign(const_iterator hint, key_type&& k, M&& obj) -> iterator {
        if (auto pos = find(k); pos == end()) {
            return emplace_hint(hint, std::move(k), std::move(obj));
        } else {
            pos->second = std::move(obj);
            return pos;
        }
    }

    template<class... Args>
    auto emplace(Args&& ... args) -> pair<iterator, bool> {
        return tree_.emplace_unique(std::forward<Args>(args)...);
    }

    template<class... Args>
    auto emplace_hint(const_iterator hint, Args&& ... args) -> iterator {
        return tree_.emplace_unique_hint(hint, std::forward<Args>(args)...);
    }

    template<class... Args>
    auto try_emplace(const key_type& k, Args&& ... args) -> pair<iterator, bool> {
        if (auto pos = find(k); pos == end()) {
            return emplace(piecewise_construct, ciel::forward_as_tuple(k),
                                                ciel::forward_as_tuple(std::forward<Args>(args)...));
        } else {
            return {pos, false};
        }
    }

    template<class... Args>
    auto try_emplace(key_type&& k, Args&& ... args) -> pair<iterator, bool> {
        if (auto pos = find(k); pos == end()) {
            return emplace(piecewise_construct, ciel::forward_as_tuple(std::move(k)),
                                                ciel::forward_as_tuple(std::forward<Args>(args)...));
        } else {
            return {pos, false};
        }
    }

    template<class... Args>
    auto try_emplace(const_iterator hint, const key_type& k, Args&& ... args) -> iterator {
        if (auto pos = find(k); pos == end()) {
            return emplace_hint(hint, piecewise_construct, ciel::forward_as_tuple(k),
                                                           ciel::forward_as_tuple(std::forward<Args>(args)...));
        } else {
            return pos;
        }
    }

    template<class... Args>
    auto try_emplace(const_iterator hint, key_type&& k, Args&& ... args) -> iterator {
        if (auto pos = find(k); pos == end()) {
            return emplace_hint(hint, piecewise_construct, ciel::forward_as_tuple(std::move(k)),
                                                           ciel::forward_as_tuple(std::forward<Args>(args)...));
        } else {
            return pos;
        }
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

    auto swap(map& other)
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
        return key_compare();
    }

    [[nodiscard]] auto value_comp() const -> value_compare {
        return tree_.value_comp();
    }

};  // class map

template<class Key, class T, class Compare, class Alloc>
[[nodiscard]] auto operator==(const map<Key, T, Compare, Alloc>& lhs, const map<Key, T, Compare, Alloc>& rhs) -> bool {
    if (lhs.size() != rhs.size()) {
        return false;
    }
    return equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<class Key, class T, class Compare, class Alloc, class Pred>
auto erase_if(map<Key, T, Compare, Alloc>& c, Pred pred) -> typename map<Key, T, Compare, Alloc>::size_type {
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
    class Comp = less<remove_const_t<typename iterator_traits<Iter>::value_type::first_type>>,
        class Alloc = allocator<pair<add_const_t<typename iterator_traits<Iter>::value_type::first_type>,
            typename iterator_traits<Iter>::value_type::second_type>>>
map(Iter, Iter, Comp = Comp(), Alloc = Alloc())
    -> map<remove_const_t<typename iterator_traits<Iter>::value_type::first_type>,
        typename iterator_traits<Iter>::value_type::second_type, Comp, Alloc>;

template<class Key, class T, class Comp = less<Key>, class Alloc = allocator<pair<const Key, T>>>
map(std::initializer_list<pair<const Key, T>>, Comp = Comp(), Alloc = Alloc()) -> map<Key, T, Comp, Alloc>;

template<legacy_input_iterator Iter, class Alloc>
map(Iter, Iter, Alloc)
    -> map<remove_const_t<typename iterator_traits<Iter>::value_type::first_type>,
        typename iterator_traits<Iter>::value_type::second_type,
            less<remove_const_t<typename iterator_traits<Iter>::value_type::first_type>>, Alloc>;

template<class Key, class T, class Allocator>
map(std::initializer_list<pair<const Key, T>>, Allocator) -> map<Key, T, less<Key>, Allocator>;

NAMESPACE_CIEL_END

namespace std {

template<class Key, class T, class Compare, class Alloc>
auto swap(ciel::map<Key, T, Compare, Alloc>& lhs,
          ciel::map<Key, T, Compare, Alloc>& rhs) noexcept(noexcept(lhs.swap(rhs))) -> void {
    lhs.swap(rhs);
}

}   // namespace std

#endif // CIELUTILS_INCLUDE_CIEL_MAP_HPP_