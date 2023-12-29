#ifndef CIELUTILS_INCLUDE_CIEL_UNORDERED_SET_H
#define CIELUTILS_INCLUDE_CIEL_UNORDERED_SET_H

#include <ciel/config.hpp>
#include <ciel/hashtable.hpp>
#include <ciel/memory_impl/allocator.hpp>

NAMESPACE_CIEL_BEGIN

template<class Key, class Hash = std::hash<Key>, class KeyEqual = equal_to<Key>, class Allocator = allocator<Key>>
class unordered_set {
public:
    using key_type             = Key;
    using value_type           = Key;
    using size_type	           = size_t;
    using difference_type      = ptrdiff_t;
    using hasher               = Hash;
    using key_equal            = KeyEqual;
    using allocator_type       = Allocator;
    using reference            = value_type&;
    using const_reference      = const value_type&;
    using pointer              = typename allocator_traits<allocator_type>::pointer;
    using const_pointer        = typename allocator_traits<allocator_type>::const_pointer;

private:
    using table_type           = hashtable<value_type, hasher, key_equal, allocator_type>;
    using alloc_traits         = allocator_traits<allocator_type>;

public:
    using iterator             = typename table_type::iterator;
    using const_iterator       = typename table_type::const_iterator;
    using local_iterator       = typename table_type::local_iterator;
    using const_local_iterator = typename table_type::const_local_iterator;

private:
    table_type table_;

public:
    unordered_set()
        : unordered_set(0) {}

    explicit unordered_set(const size_type bucket_count,
                           const hasher& hash = hasher(),
                           const key_equal& equal = key_equal(),
                           const allocator_type& alloc = allocator_type())
        : table_(bucket_count, hash, equal, alloc) {}

    unordered_set(const size_type bucket_count,
                  const allocator_type& alloc)
        : unordered_set(bucket_count, hasher(), key_equal(), alloc) {}

    unordered_set(const size_type bucket_count,
                  const hasher& hash,
                  const allocator_type& alloc)
        : unordered_set(bucket_count, hash, key_equal(), alloc) {}

    explicit unordered_set(const allocator_type& alloc)
        : unordered_set(0, hasher(), key_equal(), alloc) {}

    template<legacy_input_iterator Iter>
    unordered_set(Iter first, Iter last,
                  const size_type bucket_count = 0,
                  const hasher& hash = hasher(),
                  const key_equal& equal = key_equal(),
                  const allocator_type& alloc = allocator_type())
        : table_(false_type{}, first, last, bucket_count, hash, equal, alloc) {}

    template<legacy_input_iterator Iter>
    unordered_set(Iter first, Iter last,
                  const size_type bucket_count,
                  const allocator_type& alloc)
        : unordered_set(first, last, bucket_count, hasher(), key_equal(), alloc) {}

    template<legacy_input_iterator Iter>
    unordered_set(Iter first, Iter last,
                  const size_type bucket_count,
                  const hasher& hash,
                  const allocator_type& alloc)
        : unordered_set(first, last, bucket_count, hash, key_equal(), alloc) {}

    unordered_set(const unordered_set& other)
        : table_(other.table_) {}

    unordered_set(const unordered_set& other,
                  const allocator_type& alloc)
        : table_(other.table_, alloc) {}

    unordered_set(unordered_set&& other) noexcept
        : table_(std::move(other.table_)) {}

    unordered_set(unordered_set&& other,
                  const allocator_type& alloc) noexcept
        : table_(std::move(other.table_), alloc) {}

    unordered_set(std::initializer_list<value_type> init,
                  const size_type bucket_count = 0,
                  const hasher& hash = hasher(),
                  const key_equal& equal = key_equal(),
                  const allocator_type& alloc = allocator_type())
        : unordered_set(init.begin(), init.end(), bucket_count, hash, equal, alloc) {}

    unordered_set(std::initializer_list<value_type> init,
                  const size_type bucket_count,
                  const allocator_type& alloc)
        : unordered_set(init, bucket_count, hasher(), key_equal(), alloc) {}

    unordered_set(std::initializer_list<value_type> init,
                  const size_type bucket_count,
                  const hasher& hash,
                  const allocator_type& alloc)
        : unordered_set(init, bucket_count, hash, key_equal(), alloc) {}

    ~unordered_set() = default;

    auto operator=(const unordered_set& other) -> unordered_set& = default;

    auto operator=(unordered_set&& other)
        noexcept(alloc_traits::is_always_equal::value &&
                 is_nothrow_move_assignable_v<hasher> &&
                 is_nothrow_move_assignable_v<key_equal>)
        -> unordered_set& = default;

    auto operator=(std::initializer_list<value_type> ilist) -> unordered_set& {
        clear();
        table_.range_insert_unique(ilist.begin(), ilist.end());
        return *this;
    }

    [[nodiscard]] auto get_allocator() const noexcept -> allocator_type {
        return table_.get_allocator();
    }

    [[nodiscard]] auto begin() noexcept -> iterator {
        return table_.begin();
    }

    [[nodiscard]] auto begin() const noexcept -> const_iterator {
        return table_.begin();
    }

    [[nodiscard]] auto cbegin() const noexcept -> const_iterator {
        return begin();
    }

    [[nodiscard]] auto end() noexcept -> iterator {
        return table_.end();
    }

    [[nodiscard]] auto end() const noexcept -> const_iterator {
        return table_.end();
    }

    [[nodiscard]] auto cend() const noexcept -> const_iterator {
        return end();
    }

    [[nodiscard]] auto empty() const noexcept -> bool {
        return table_.empty();
    }

    [[nodiscard]] auto size() const noexcept -> size_type {
        return table_.size();
    }

    [[nodiscard]] auto max_size() const noexcept -> size_type {
        return table_.max_size();
    }

    auto clear() noexcept -> void {
        table_.clear();
    }

    auto insert(const value_type& value) -> pair<iterator, bool> {
        return emplace(value);
    }

    auto insert(value_type&& value) -> pair<iterator, bool> {
        return emplace(std::move(value));
    }

//  iterator insert(iterator hint, const value_type& value) {
//      return emplace_hint(hint, value);
//  }
//
//  iterator insert(iterator hint, value_type&& value) {
//      return emplace_hint(hint, std::move(value));
//  }

    template<legacy_input_iterator Iter>
    auto insert(Iter first, Iter last) -> void {
        table_.range_insert_unique(first, last);
    }

    auto insert(std::initializer_list<value_type> ilist) -> void {
        insert(ilist.begin(), ilist.end());
    }

    template<class... Args>
    auto emplace(Args&&... args) -> pair<iterator, bool> {
        return table_.emplace_unique(std::forward<Args>(args)...);
    }

//  template<class... Args>
//  iterator emplace_hint(iterator hint, Args&&... args);

    auto erase(iterator pos) -> iterator {
        return table_.erase(pos);
    }

    auto erase(iterator first, iterator last) -> iterator {
        return table_.erase(first, last);
    }

    auto erase(const Key& key) -> size_type {
        return table_.erase_unique(key);
    }

    template<class K>
        requires requires { typename hasher::is_transparent; typename key_equal::is_transparent; }
    auto erase(K&& x) -> size_type {
        return table_.erase_unique(x);
    }

    auto swap(unordered_set& other)
        noexcept(alloc_traits::is_always_equal::value &&
                 is_nothrow_swappable_v<hasher> &&
                 is_nothrow_swappable_v<key_equal>) -> void {
        table_.swap(other.table_);
    }

    [[nodiscard]] auto count(const Key& key) const -> size_type {
        return table_.count_unique(key);
    }

    template<class K>
        requires requires { typename hasher::is_transparent; typename key_equal::is_transparent; }
    [[nodiscard]] auto count(const K& x) const -> size_type {
        return table_.count_unique(x);
    }

    [[nodiscard]] auto find(const Key& key) -> iterator {
        return table_.find(key);
    }

    [[nodiscard]] auto find(const Key& key) const -> const_iterator {
        return table_.find(key);
    }

    template<class K>
        requires requires { typename hasher::is_transparent; typename key_equal::is_transparent; }
    [[nodiscard]] auto find(const K& x) -> iterator {
        return table_.find(x);
    }

    template<class K>
        requires requires { typename hasher::is_transparent; typename key_equal::is_transparent; }
    [[nodiscard]] auto find(const K& x) const -> const_iterator {
        return table_.find(x);
    }

    [[nodiscard]] auto contains(const Key& key) const -> bool {
        return table_.contains(key);
    }

    template<class K>
        requires requires { typename hasher::is_transparent; typename key_equal::is_transparent; }
    [[nodiscard]] auto contains(const K& x) const -> bool {
        return table_.contains(x);
    }

    [[nodiscard]] auto equal_range(const Key& key) -> pair<iterator,iterator> {
        return table_.equal_range(key);
    }

    [[nodiscard]] auto equal_range( const Key& key ) const -> pair<const_iterator,const_iterator> {
        return table_.equal_range(key);
    }

    template<class K>
        requires requires { typename hasher::is_transparent; typename key_equal::is_transparent; }
    [[nodiscard]] auto equal_range(const K& x) -> pair<iterator,iterator> {
        return table_.equal_range(x);
    }

    template<class K>
        requires requires { typename hasher::is_transparent; typename key_equal::is_transparent; }
    [[nodiscard]] auto equal_range(const K& x) const -> pair<const_iterator,const_iterator> {
        return table_.equal_range(x);
    }

    [[nodiscard]] auto begin(const size_type n) -> local_iterator {
        return table_.begin(n);
    }

    [[nodiscard]] auto begin(const size_type n) const -> const_local_iterator {
        return table_.begin(n);
    }

    [[nodiscard]] auto cbegin(const size_type n) const -> const_local_iterator {
        return begin(n);
    }

    [[nodiscard]] auto end(const size_type n) -> local_iterator {
        return table_.end(n);
    }

    [[nodiscard]] auto end(const size_type n) const -> const_local_iterator {
        return table_.end(n);
    }

    [[nodiscard]] auto cend(const size_type n) const -> const_local_iterator {
        return end(n);
    }

    [[nodiscard]] auto bucket_count() const -> size_type {
        return table_.bucket_count();
    }

    [[nodiscard]] auto max_bucket_count() const -> size_type {
        return max_size();
    }

    [[nodiscard]] auto bucket_size(const size_type n) const -> size_type {
        return table_.bucket_size(n);
    }

    [[nodiscard]] auto bucket(const Key& key) const -> size_type {
        return table_.bucket(key);
    }

    [[nodiscard]] auto load_factor() const -> float {
        return table_.load_factor();
    }

    [[nodiscard]] auto max_load_factor() const -> float {
        return table_.max_load_factor();
    }

    auto max_load_factor(const float ml) -> void {
        table_.max_load_factor(ml);
    }

    auto rehash(const size_type count) -> void {
        table_.rehash(count);
    }

    auto reserve(const size_type count) -> void {
        rehash(std::ceil(count / max_load_factor()));
    }

    [[nodiscard]] auto hash_function() const -> hasher {
        return table_.hash_function();
    }

    [[nodiscard]] auto key_eq() const -> key_equal {
        return table_.key_eq();
    }

};  // class unordered_set

template<class Key, class Hash, class KeyEqual, class Allocator>
[[nodiscard]] auto operator==(const unordered_set<Key,Hash,KeyEqual,Allocator>& lhs,
                              const unordered_set<Key,Hash,KeyEqual,Allocator>& rhs) -> bool {
    if (lhs.size() != rhs.size()) {
        return false;
    }
    for (auto it = lhs.begin(); it != lhs.end(); ++it) {
        if (!rhs.contains(*it)) {
            return false;
        }
    }
    return true;
}

template<class Key, class Hash, class KeyEqual, class Alloc, class Pred>
auto erase_if(unordered_set<Key, Hash, KeyEqual, Alloc>& c, Pred pred)
    -> typename unordered_set<Key, Hash, KeyEqual, Alloc>::size_type {
    auto old_size = c.size();
    for (auto i = c.begin(), last = c.end(); i != last; ) {
        if (pred(*i)) {
            i = c.erase(i);
        } else {
            ++i;
        }
    }
    return old_size - c.size();
}

template<class Iter, class Hash = std::hash<typename iterator_traits<Iter>::value_type>,
         class Pred = equal_to<typename iterator_traits<Iter>::value_type>,
         class Alloc = allocator<typename iterator_traits<Iter>::value_type>>
unordered_set(Iter, Iter,
              typename allocator_traits<Alloc>::size_type = 0,
              Hash = Hash(), Pred = Pred(), Alloc = Alloc())
    -> unordered_set<typename iterator_traits<Iter>::value_type, Hash, Pred, Alloc>;

template<class T, class Hash = std::hash<T>, class Pred = equal_to<T>, class Alloc = allocator<T>>
unordered_set(std::initializer_list<T>,
              typename allocator_traits<Alloc>::size_type = 0,
              Hash = Hash(), Pred = Pred(), Alloc = Alloc())
    -> unordered_set<T, Hash, Pred, Alloc>;

template<class Iter, class Alloc>
unordered_set(Iter, Iter, typename allocator_traits<Alloc>::size_type, Alloc)
    -> unordered_set<typename iterator_traits<Iter>::value_type,
                     std::hash<typename iterator_traits<Iter>::value_type>,
                     equal_to<typename iterator_traits<Iter>::value_type>, Alloc>;

template<class Iter, class Hash, class Alloc>
unordered_set(Iter, Iter, typename allocator_traits<Alloc>::size_type, Hash, Alloc)
    -> unordered_set<typename iterator_traits<Iter>::value_type, Hash,
                     equal_to<typename iterator_traits<Iter>::value_type>, Alloc>;

template<class T, class Alloc>
unordered_set(std::initializer_list<T>, typename allocator_traits<Alloc>::size_type, Alloc)
    -> unordered_set<T, std::hash<T>, equal_to<T>, Alloc>;

template<class T, class Hash, class Alloc>
unordered_set(std::initializer_list<T>, typename allocator_traits<Alloc>::size_type, Hash, Alloc)
    -> unordered_set<T, Hash, equal_to<T>, Alloc>;

NAMESPACE_CIEL_END

namespace std {

template<class Key, class Hash, class KeyEqual, class Alloc>
auto swap(ciel::unordered_set<Key, Hash, KeyEqual, Alloc>& lhs,
          ciel::unordered_set<Key, Hash, KeyEqual, Alloc>& rhs) noexcept(noexcept(lhs.swap(rhs))) -> void {
    lhs.swap(rhs);
}

}   // namespace std

#endif // CIELUTILS_INCLUDE_CIEL_UNORDERED_SET_H