#ifndef CIELUTILS_INCLUDE_CIEL_HASHTABLE_HPP_
#define CIELUTILS_INCLUDE_CIEL_HASHTABLE_HPP_

#include <ciel/algorithm_impl/max.hpp>
#include <ciel/algorithm_impl/upper_bound.hpp>
#include <ciel/array.hpp>
#include <ciel/config.hpp>
#include <ciel/iterator_impl/distance.hpp>
#include <ciel/memory_impl/allocator_traits.hpp>
#include <ciel/memory_impl/unique_ptr.hpp>
#include <ciel/utility_impl/pair.hpp>
#include <cmath>

NAMESPACE_CIEL_BEGIN

// hashtable is implemented as adjacency lists (of prime size), when number of elements is beyond the mlf,
// we allocate a larger block of memory and tranfer nodes.
//
// C++ named requirements: UnorderedAssociativeContainer:
//      If two Keys are equal according to Pred, Hash must return the same value for both keys.
//
// begin()'s complexity is constant in standard, so common implementation maintain a start pointer in hashtable,
// we haven't followed this standard.

constexpr array<size_t, 256> PrimeNumberArray =
    {2u, 3u, 5u, 7u, 11u, 13u, 17u, 19u, 23u, 29u, 31u, 37u, 41u, 43u, 47u, 53u, 59u, 61u, 67u, 71u, 73u, 79u, 83u, 89u,
     97u, 103u, 109u, 113u, 127u, 137u, 139u, 149u, 157u, 167u, 179u, 193u, 199u, 211u, 227u, 241u, 257u, 277u, 293u,
     313u, 337u, 359u, 383u, 409u, 439u, 467u, 503u, 541u, 577u, 619u, 661u, 709u, 761u, 823u, 887u, 953u, 1031u, 1109u,
     1193u, 1289u, 1381u, 1493u, 1613u, 1741u, 1879u, 2029u, 2179u, 2357u, 2549u, 2753u, 2971u, 3209u, 3469u, 3739u,
     4027u, 4349u, 4703u, 5087u, 5503u, 5953u, 6427u, 6949u, 7517u, 8123u, 8783u, 9497u, 10273u, 11113u, 12011u, 12983u,
     14033u, 15173u, 16411u, 17749u, 19183u, 20753u, 22447u, 24281u, 26267u, 28411u, 30727u, 33223u, 35933u, 38873u,
     42043u, 45481u, 49201u, 53201u, 57557u, 62233u, 67307u, 72817u, 78779u, 85229u, 92203u, 99733u, 107897u, 116731u,
     126271u, 136607u, 147793u, 159871u, 172933u, 187091u, 202409u, 218971u, 236897u, 256279u, 277261u, 299951u,
     324503u, 351061u, 379787u, 410857u, 444487u, 480881u, 520241u, 562841u, 608903u, 658753u, 712697u, 771049u,
     834181u, 902483u, 976369u, 1056323u, 1142821u, 1236397u, 1337629u, 1447153u, 1565659u, 1693859u, 1832561u,
     1982627u, 2144977u, 2320627u, 2510653u, 2716249u, 2938679u, 3179303u, 3439651u, 3721303u, 4026031u, 4355707u,
     4712381u, 5098259u, 5515729u, 5967347u, 6456007u, 6984629u, 7556579u, 8175383u, 8844859u, 9569143u, 10352717u,
     11200489u, 12117689u, 13109983u, 14183539u, 15345007u, 16601593u, 17961079u, 19431899u, 21023161u, 22744717u,
     24607243u, 26622317u, 28802401u, 31160981u, 33712729u, 36473443u, 39460231u, 42691603u, 46187573u, 49969847u,
     54061849u, 58488943u, 63278561u, 68460391u, 74066549u, 80131819u, 86693767u, 93793069u, 101473717u, 109783337u,
     118773397u, 128499677u, 139022417u, 150406843u, 162723577u, 176048909u, 190465427u, 206062531u, 222936881u,
     241193053u, 260944219u, 282312799u, 305431229u, 330442829u, 357502601u, 386778277u, 418451333u, 452718089u,
     489790921u, 529899637u, 573292817u, 620239453u, 671030513u, 725980837u, 785430967u, 849749479u, 919334987u,
     994618837u, 1076067617u, 1164186217u, 1259520799u, 1362662261u, 1474249943u, 1594975441u, 1725587117u, 1866894511u,
     2019773507u, 2185171673u, 2364114217u, 2557710269u, 2767159799u, 2993761039u, 3238918481u, 3504151727u,
     3791104843u, 4101556399u, 4294967291u};

[[nodiscard]] inline auto next_prime(const size_t n) noexcept -> size_t {
    if (n >= PrimeNumberArray.back()) {
        return n;
    }
    return *upper_bound(PrimeNumberArray.begin(), PrimeNumberArray.end(), n);
}

template<class T>
struct hashtable_node {
    hashtable_node* next_;
    T value_;
    size_t hash_;

    template<class Hash, class... Args>
    hashtable_node(hashtable_node* n, const Hash& h, Args&& ... args)
        : next_(n), value_(std::forward<Args>(args)...), hash_(h(value_)) {}

}; // struct hashtable_node

template<class Allocator>
class bucket_list_deallocator {
private:
    using allocator_type = Allocator;
    using alloc_traits   = allocator_traits<allocator_type>;
    using size_type      = typename alloc_traits::size_type;
    using pointer        = typename alloc_traits::pointer;

    size_type size_;
    [[no_unique_address]] allocator_type allocator_;

    template<class, class, class, class>
    friend class hashtable;

public:
    bucket_list_deallocator(size_type s, const allocator_type& alloc = allocator_type()) noexcept
        : size_(s), allocator_(alloc) {}

    [[nodiscard]] auto size() const noexcept -> size_type {
        return size_;
    }

    auto operator()(pointer p) noexcept -> void {
        alloc_traits::deallocate(allocator_, p, size_);
    }

};  // class bucket_list_deallocator

template<class T, class Pointer, class Reference, class Allocator>
class hashtable_iterator {
public:
    using difference_type   = ptrdiff_t;
    using value_type        = T;
    using pointer           = Pointer;
    using reference         = Reference;
    using iterator_category = forward_iterator_tag;
    using iterator_concept  = forward_iterator_tag;

private:
    using node_type         = hashtable_node<value_type>;

    using size_type         = typename allocator_traits<Allocator>::size_type;
    using bucket_list_type  = unique_ptr<node_type*[], bucket_list_deallocator<Allocator>>;

    node_type* it_;
    const bucket_list_type& bucket_list_;
    size_type index_;

    template<class, class, class, class> friend class hashtable_iterator;

public:
    [[nodiscard]] auto size() const noexcept -> size_type {
        return bucket_list_.get_deleter().size();
    }

    [[nodiscard]] auto index() const noexcept -> size_type {
        return index_;
    }

    hashtable_iterator() noexcept = default;

    hashtable_iterator(const node_type* p, const bucket_list_type& bl, size_type i) noexcept
        : it_(const_cast<node_type*>(p)), bucket_list_(bl), index_(i) {}

    hashtable_iterator(const hashtable_iterator&) noexcept = default;
    hashtable_iterator(hashtable_iterator&&) noexcept = default;

    template<class P, class R>
    explicit hashtable_iterator(const hashtable_iterator<T, P, R, Allocator>& other) noexcept
        : it_(const_cast<node_type*>(other.it_)), bucket_list_(other.bucket_list_), index_(other.index_) {}

    ~hashtable_iterator() = default;

    auto operator=(const hashtable_iterator&) noexcept -> hashtable_iterator& = default;
    auto operator=(hashtable_iterator&&) noexcept -> hashtable_iterator& = default;

    [[nodiscard]] auto next() const noexcept -> hashtable_iterator {
        hashtable_iterator res(*this);
        ++res;
        return res;
    }

    [[nodiscard]] auto operator*() const noexcept -> reference {
        return it_->value_;
    }

    [[nodiscard]] auto operator->() const noexcept -> pointer {
        return &it_->value_;
    }

    auto operator++() noexcept -> hashtable_iterator& {
        if (it_->next_) {
            it_ = it_->next_;
        } else {
            ++index_;
            while (index_ != size() && !bucket_list_[index_]) {
                ++index_;
            }
            if (index_ == size()) {
                it_ = nullptr;
            } else {
                it_ = bucket_list_[index_];
            }
        }
        return *this;
    }

    [[nodiscard]] auto operator++(int) noexcept -> hashtable_iterator {
        hashtable_iterator res(*this);
        ++*this;
        return res;
    }

    [[nodiscard]] auto base() const noexcept -> node_type* {
        return it_;
    }

    [[nodiscard]] explicit operator bool() const noexcept {
        return it_ != nullptr;
    }

};  // class hashtable_iterator

template<class T, class Pointer1, class Pointer2, class Reference1, class Reference2, class Allocator>
auto operator==(const hashtable_iterator<T, Pointer1, Reference1, Allocator>& lhs,
                const hashtable_iterator<T, Pointer2, Reference2, Allocator>& rhs) noexcept -> bool {
    return lhs.base() == rhs.base();
}

template<class T, class Pointer1, class Pointer2, class Reference1, class Reference2, class Allocator>
auto operator!=(const hashtable_iterator<T, Pointer1, Reference1, Allocator>& lhs,
                const hashtable_iterator<T, Pointer2, Reference2, Allocator>& rhs) noexcept -> bool {
    return !(lhs.base() == rhs.base());
}

template<class T, class Pointer, class Reference>
class hashtable_local_iterator {
public:
    using difference_type   = ptrdiff_t;
    using value_type        = T;
    using pointer           = Pointer;
    using reference         = Reference;
    using iterator_category = forward_iterator_tag;
    using iterator_concept  = forward_iterator_tag;

private:
    using node_type         = hashtable_node<value_type>;

    node_type* it_;

public:
    hashtable_local_iterator() noexcept = default;

    explicit hashtable_local_iterator(const node_type* p) noexcept : it_(const_cast<node_type*>(p)) {}

    hashtable_local_iterator(const hashtable_local_iterator&) noexcept = default;
    hashtable_local_iterator(hashtable_local_iterator&&) noexcept = default;

    template<class P, class R>
    hashtable_local_iterator(const hashtable_local_iterator<T, P, R>& other) noexcept
        : it_(const_cast<node_type*>(other.base())) {}

    ~hashtable_local_iterator() = default;

    auto operator=(const hashtable_local_iterator&) noexcept -> hashtable_local_iterator& = default;
    auto operator=(hashtable_local_iterator&&) noexcept -> hashtable_local_iterator& = default;

    [[nodiscard]] auto next() const noexcept -> hashtable_local_iterator {
        hashtable_local_iterator res(*this);
        ++res;
        return res;
    }

    [[nodiscard]] auto operator*() const noexcept -> reference {
        return it_->value_;
    }

    [[nodiscard]] auto operator->() const noexcept -> pointer {
        return &it_->value_;
    }

    auto operator++() noexcept -> hashtable_local_iterator& {
        it_ = it_->next_;
        return *this;
    }

    [[nodiscard]] auto operator++(int) noexcept -> hashtable_local_iterator {
        hashtable_local_iterator res(*this);
        ++*this;
        return res;
    }

    [[nodiscard]] auto base() const noexcept -> node_type* {
        return it_;
    }

    [[nodiscard]] explicit operator bool() const noexcept {
        return it_ != nullptr;
    }

};  // class hashtable_local_iterator

template<class T, class Pointer1, class Pointer2, class Reference1, class Reference2>
[[nodiscard]] auto operator==(const hashtable_local_iterator<T, Pointer1, Reference1>& lhs,
                              const hashtable_local_iterator<T, Pointer2, Reference2>& rhs) noexcept -> bool {
    return lhs.base() == rhs.base();
}

template<class T, class Pointer1, class Pointer2, class Reference1, class Reference2>
[[nodiscard]] auto operator!=(const hashtable_local_iterator<T, Pointer1, Reference1>& lhs,
                              const hashtable_local_iterator<T, Pointer2, Reference2>& rhs) noexcept -> bool {
    return !(lhs.base() == rhs.base());
}

template<class T, class Hash, class Equal, class Allocator>
class hashtable {
public:
    using value_type           = T;
    using size_type            = size_t;
    using difference_type      = ptrdiff_t;
    using hasher               = Hash;
    using key_equal            = Equal;
    using allocator_type       = Allocator;
    using reference            = value_type&;
    using const_reference      = const value_type&;
    using pointer              = typename allocator_traits<allocator_type>::pointer;
    using const_pointer        = typename allocator_traits<allocator_type>::const_pointer;

private:
    using node_type            = hashtable_node<value_type>;

    using alloc_traits         = allocator_traits<allocator_type>;
    using node_allocator       = typename alloc_traits::template rebind_alloc<node_type>;
    using node_alloc_traits    = typename alloc_traits::template rebind_traits<node_type>;

    using pointer_allocator    = typename alloc_traits::template rebind_alloc<node_type*>;
    using pointer_alloc_traits = typename alloc_traits::template rebind_traits<node_type*>;

public:
    using iterator             = hashtable_iterator<value_type, pointer, reference, pointer_allocator>;
    using const_iterator       = hashtable_iterator<value_type, const_pointer, const_reference, pointer_allocator>;

    using local_iterator       = hashtable_local_iterator<value_type, pointer, reference>;
    using const_local_iterator = hashtable_local_iterator<value_type, const_pointer, const_reference>;

private:
    using bucket_list_type     = unique_ptr<node_type*[], bucket_list_deallocator<pointer_allocator>>;

    bucket_list_type bucket_list_;
    size_type size_;
    float mlf_;
    [[no_unique_address]] node_allocator allocator_;
    [[no_unique_address]] hasher hasher_;
    [[no_unique_address]] key_equal ke_;

    template<class... Args>
    [[nodiscard]] auto allocate_and_construct_node(Args&& ... args) -> node_type* {
        node_type* new_node = node_alloc_traits::allocate(allocator_, 1);
        CIEL_TRY {
            node_alloc_traits::construct(allocator_, new_node, nullptr, hasher_, std::forward<Args>(args)...);
        } CIEL_CATCH (...) {
            node_alloc_traits::deallocate(allocator_, new_node, 1);
            CIEL_THROW;
        }
        ++size_;
        return new_node;
    }

    auto destroy_and_deallocate_node(node_type* node) noexcept -> void {
        node_alloc_traits::destroy(allocator_, node);
        node_alloc_traits::deallocate(allocator_, node, 1);
        --size_;
    }

    auto do_rehash(size_type count) -> void {
        auto& pointer_alloc = bucket_list_.get_deleter().allocator_;
        bucket_list_type new_bucket_list(pointer_alloc_traits::allocate(pointer_alloc, count), {count, pointer_alloc});
        for (size_type i = 0; i < count; ++i) {
            new_bucket_list[i] = nullptr;
        }
        for (auto it = begin(); it != end(); ++it) {
            it.base()->next_ = new_bucket_list[it.base()->hash_ % count];
            new_bucket_list[it.base()->hash_ % count] = it.base();
        }
        bucket_list_ = std::move(new_bucket_list);
    }

public:
    hashtable(size_type bucket_count, const hasher& hash, const key_equal& equal, const allocator_type& alloc)
        : bucket_list_(nullptr, {0}), size_(0), mlf_(1.0F), allocator_(alloc), hasher_(hash), ke_(equal) {
        if (bucket_count > 0) {
            auto& pointer_alloc = bucket_list_.get_deleter().allocator_;
            bucket_list_ = bucket_list_type(pointer_alloc_traits::allocate(pointer_alloc, bucket_count),
                                            {bucket_count, pointer_alloc});
        }
    }

    template<legacy_input_iterator Iter>
    hashtable(true_type /*unused*/, Iter first, Iter last, size_type bucket_count,
              const hasher& hash, const key_equal& equal, const allocator_type& alloc)
        : bucket_list_(nullptr, {0}), size_(0), mlf_(1.0F), allocator_(alloc), hasher_(hash), ke_(equal) {
        if (bucket_count > 0) {
            auto& pointer_alloc = bucket_list_.get_deleter().allocator_;
            bucket_list_ = bucket_list_type(pointer_alloc_traits::allocate(pointer_alloc, bucket_count),
                                            {bucket_count, pointer_alloc});
        }
        range_insert_multi(first, last);
    }

    template<legacy_input_iterator Iter>
    hashtable(false_type /*unused*/, Iter first, Iter last, size_type bucket_count,
              const hasher& hash, const key_equal& equal, const allocator_type& alloc)
        : bucket_list_(nullptr, {0}), size_(0), mlf_(1.0F), allocator_(alloc), hasher_(hash), ke_(equal) {
        if (bucket_count > 0) {
            auto& pointer_alloc = bucket_list_.get_deleter().allocator_;
            bucket_list_ = bucket_list_type(pointer_alloc_traits::allocate(pointer_alloc, bucket_count),
                                            {bucket_count, pointer_alloc});
        }
        range_insert_unique(first, last);
    }

    hashtable(const hashtable& other)
        : bucket_list_(nullptr, {0}), size_(0), mlf_(other.mlf_),
          allocator_(node_alloc_traits::select_on_container_copy_construction(other.get_allocator())),
          hasher_(other.hasher_), ke_(other.ke_) {
        if (other.bucket_count() > 0) {
            auto& pointer_alloc = bucket_list_.get_deleter().allocator_;
            bucket_list_ = bucket_list_type(pointer_alloc_traits::allocate(pointer_alloc, other.bucket_count()),
                                            {other.bucket_count(), pointer_alloc});
        }
        range_insert_multi(other.begin(), other.end());
    }

    hashtable(const hashtable& other, const allocator_type& alloc)
        : bucket_list_(nullptr, {0}), size_(0), mlf_(other.mlf_),
          allocator_(alloc),
          hasher_(other.hasher_),
          ke_(other.ke_) {
        if (other.bucket_count() > 0) {
            auto& pointer_alloc = bucket_list_.get_deleter().allocator_;
            bucket_list_ = bucket_list_type(pointer_alloc_traits::allocate(pointer_alloc, other.bucket_count()),
                                            {other.bucket_count(), pointer_alloc});
        }
        range_insert_multi(other.begin(), other.end());
    }

    hashtable(hashtable&& other) noexcept
        : bucket_list_(std::move(other.bucket_list_)), size_(other.size_), mlf_(other.mlf_),
          allocator_(std::move(other.allocator_)),
          hasher_(std::move(other.hasher_)),
          ke_(std::move(other.ke_)) {
        other.size_ = 0;
    }

    hashtable(hashtable&& other, const allocator_type& alloc) noexcept
        : bucket_list_(std::move(other.bucket_list_)), size_(other.size_), mlf_(other.mlf_),
          allocator_(alloc),
          hasher_(std::move(other.hasher_)),
          ke_(std::move(other.ke_)) {
        other.size_ = 0;
    }

    ~hashtable() {
        clear();
    }

    auto operator=(const hashtable& other) -> hashtable& {
        if (this == addressof(other)) {
            return *this;
        }
        if (alloc_traits::propagate_on_container_copy_assignment::value) {
            if (allocator_ != other.allocator_) {
                hashtable(other.allocator_).swap(*this);
                range_insert_multi(other.begin(), other.end());
                return *this;
            }
            allocator_ = other.allocator_;
        }
        clear();
        range_insert_multi(other.begin(), other.end());
        return *this;
    }

    auto operator=(hashtable&& other) noexcept -> hashtable& {
        if (this == addressof(other)) {
            return *this;
        }
        if (!alloc_traits::propagate_on_container_move_assignment::value && allocator_ != other.allocator_) {
            clear();
            range_insert_multi(other.begin(), other.end());
            return *this;
        }
        if (alloc_traits::propagate_on_container_move_assignment::value) {
            allocator_ = std::move(other.allocator_);
        }
        clear();
        bucket_list_ = std::move(other.bucket_list_);
        size_ = other.size_;
        mlf_ = other.mlf_;
        hasher_ = std::move(other.hasher_);
        ke_ = std::move(other.ke_) ;
        other.size_ = 0;
        return *this;
    }

    [[nodiscard]] auto get_allocator() const noexcept -> allocator_type {
        return allocator_;
    }

    [[nodiscard]] auto begin() noexcept -> iterator {
        for (size_type i = 0; i < bucket_count(); ++i) {
            if (bucket_list_[i]) {
                return iterator(bucket_list_[i], bucket_list_, i);
            }
        }
        return end();
    }

    [[nodiscard]] auto begin() const noexcept -> const_iterator {
        for (size_type i = 0; i < bucket_count(); ++i) {
            if (bucket_list_[i]) {
                return const_iterator(bucket_list_[i], bucket_list_, i);
            }
        }
        return end();
    }

    [[nodiscard]] auto end() noexcept -> iterator {
        return iterator(nullptr, bucket_list_, bucket_count());
    }

    [[nodiscard]] auto end() const noexcept -> const_iterator {
        return const_iterator(nullptr, bucket_list_, bucket_count());
    }

    [[nodiscard]] auto empty() const noexcept -> bool {
        return size() == 0;
    }

    [[nodiscard]] auto size() const noexcept -> size_type {
        return size_;
    }

    [[nodiscard]] auto max_size() const noexcept -> size_type {
        return node_alloc_traits::max_size(allocator_);
    }

    auto clear() noexcept -> void {
        for (auto it = begin(); it != end();) {
            destroy_and_deallocate_node(it++.base());
        }
        for (size_type i = 0; i < bucket_count(); ++i) {
            bucket_list_[i] = nullptr;
        }
    }

    template<legacy_input_iterator Iter>
    auto range_insert_unique(Iter first, Iter last) -> void {
        if constexpr (is_forward_iterator<Iter>::value) {
            auto len = ciel::distance(first, last);
            if (const size_type new_bucket_count = (size() + len) / max_load_factor();
                new_bucket_count > bucket_count()) {
                do_rehash(next_prime(new_bucket_count));
            }
        }
        while (first != last) {
            emplace_unique(*first++);
        }
    }

    template<legacy_input_iterator Iter>
    auto range_insert_multi(Iter first, Iter last) -> void {
        if constexpr (is_forward_iterator<Iter>::value) {
            auto len = ciel::distance(first, last);
            if (const size_type new_bucket_count = (size() + len) / max_load_factor();
                new_bucket_count > bucket_count()) {
                do_rehash(next_prime(new_bucket_count));
            }
        }
        while (first != last) {
            emplace_multi(*first++);
        }
    }

    template<class... Args>
    auto emplace_unique(Args&& ... args) -> pair<iterator, bool> {
        if (bucket_count() == 0 || load_factor() >= max_load_factor()) {
            do_rehash(next_prime(bucket_count()));
        }
        node_type* new_node = allocate_and_construct_node(std::forward<Args>(args)...);
        size_type new_node_index = new_node->hash_ % bucket_count();
        for (auto it = begin(new_node_index); it != end(new_node_index); ++it) {
            if (ke_(new_node->value_, *it)) {
                destroy_and_deallocate_node(new_node);
                return {{it.base(), bucket_list_, new_node_index}, false};
            }
        }
        new_node->next_ = bucket_list_[new_node_index];
        bucket_list_[new_node_index] = new_node;
        return {{new_node, bucket_list_, new_node_index}, true};
    }

    template<class... Args>
    auto emplace_multi(Args&& ... args) -> iterator {
        if (bucket_count() == 0 || load_factor() >= max_load_factor()) {
            do_rehash(next_prime(bucket_count()));
        }
        node_type* new_node = allocate_and_construct_node(std::forward<Args>(args)...);
        size_type new_node_index = new_node->hash_ % bucket_count();

        if (bucket_list_[new_node_index] == nullptr) {
            bucket_list_[new_node_index] = new_node;
            return {new_node, bucket_list_, new_node_index};
        }
        // find the first equal element or nullptr, insert there
        for (auto it = begin(new_node_index); ; ++it) {
            if (it.base()->next_ == nullptr || ke_(it.base()->next_->value_, new_node->value_)) {
                new_node->next_ = it.base()->next_;
                it.base()->next_ = new_node;
                return {new_node, bucket_list_, new_node_index};
            }
        }
        unreachable();
    }

//  template<class... Args>
//  iterator emplace_unique_hint(iterator hint, Args&& ... args);
//  template<class... Args>
//  iterator emplace_multi_hint(iterator hint, Args&& ... args);

    auto erase(iterator pos) noexcept -> iterator {
        iterator res(pos);
        ++res;

        size_type bucket_index = pos.index();
        // if pos is the first of current bucket
        if (pos.base() == bucket_list_[bucket_index]) {
            bucket_list_[bucket_index] = pos.base()->next_;
            destroy_and_deallocate_node(pos.base());
            return res;
        }

        // find the previous node of pos and change it's next
        node_type* before_pos = bucket_list_[bucket_index];
        for (; before_pos->next_ != pos.base(); before_pos = before_pos->next_) {}
        before_pos->next_ = pos.base()->next_;
        destroy_and_deallocate_node(pos.base());
        return res;
    }

    auto erase(iterator first, iterator last) noexcept -> iterator {
        while (first != last) {
            erase(first++);
        }
        return last;
    }

    template<class Key>
    auto erase_unique(const Key& key) -> size_type {
        iterator pos(find(key));
        if (pos == end()) {
            return 0;
        }
        erase(pos);
        return 1;
    }

    template<class Key>
    auto erase_multi(const Key& key) -> size_type {
        size_type res(0);
        iterator pos(find(key));
        if (pos != end()) {
            do {
                erase(pos++);
                ++res;
            } while (pos != end() && ke_(*pos, key));
        }
        return res;
    }

    auto swap(hashtable& other) noexcept -> void {
        using std::swap;
        swap(bucket_list_, other.bucket_list_);
        swap(size_, other.size_);
        swap(mlf_, other.mlf_);
        swap(allocator_, other.allocator_);
        swap(hasher_, other.hasher_);
        swap(ke_, other.ke_);
    }

    template<class Key>
    [[nodiscard]] auto count_unique(const Key& key) const -> size_type {
        return static_cast<size_type>(contains(key));
    }

    template<class Key>
    [[nodiscard]] auto count_multi(const Key& key) const -> size_type {
        size_type res(0);
        iterator pos(find(key));
        if (pos != end()) {
            do {
                ++pos;
                ++res;
            } while (pos != end() && ke_(*pos, key));
        }
        return res;
    }

    template<class Key>
    [[nodiscard]] auto find(const Key& key) const -> const_iterator {
        if (bucket_count() == 0) {
            return end();
        }
        const size_type hash = hasher_(key);
        const size_type bucket_index = hash % bucket_count();
        for (auto it = begin(bucket_index); it != end(bucket_index); ++it) {
            if (ke_(*it, key)) {
                return {it.base(), bucket_list_, bucket_index};
            }
        }
        return end();
    }

    template<class Key>
    [[nodiscard]] auto contains(const Key& key) const -> bool {
        return find(key) != end();
    }

    template<class Key>
    [[nodiscard]] auto equal_range(const Key& key) const -> pair<const_iterator, const_iterator> {
        iterator first = find(key);
        if (first == end()) {
            return {first, first};
        }
        iterator last = first;
        ++last;
        while (last != end() && ke_(*last, key)) {
            ++last;
        }
        return {first, last};
    }

    [[nodiscard]] auto begin(const size_type n) -> local_iterator {
        return local_iterator{bucket_list_[n]};
    }

    [[nodiscard]] auto begin(const size_type n) const -> const_local_iterator {
        return const_local_iterator{bucket_list_[n]};
    }

    [[nodiscard]] auto end(size_type /*unused*/) -> local_iterator {
        return local_iterator{nullptr};
    }

    [[nodiscard]] auto end(size_type /*unused*/) const -> const_local_iterator {
        return const_local_iterator{nullptr};
    }

    [[nodiscard]] auto bucket_count() const noexcept -> size_type {
        return bucket_list_.get_deleter().size();
    }

    [[nodiscard]] auto bucket_size(const size_type n) const -> size_type {
        size_type res = 0;
        for (auto it = begin(n); it != end(n); ++it, ++res);
        return res;
    }

    template<class Key>
    [[nodiscard]] auto bucket(const Key& key) const -> size_type {
        return hasher_(key) % bucket_count();
    }

    [[nodiscard]] auto load_factor() const noexcept -> float {
        return static_cast<float>(size()) / bucket_count();
    }

    [[nodiscard]] auto max_load_factor() const noexcept -> float {
        return mlf_;
    }

    auto max_load_factor(const float ml) noexcept -> void {
        mlf_ = ml;
    }

    auto rehash(size_type count) -> void {
        count = ciel::max<size_type>(count, std::ceil(size() / max_load_factor()));
        count = next_prime(count);
        do_rehash(count);
    }

    auto reserve(const size_type count) -> void {
        rehash(std::ceil(count / max_load_factor()));
    }

    [[nodiscard]] auto hash_function() const noexcept -> hasher {
        return hasher_;
    }

    [[nodiscard]] auto key_eq() const noexcept -> key_equal {
        return ke_;
    }

};  // class hashtable

template<class Key, class Hash, class KeyEqual, class Allocator>
[[nodiscard]] auto operator==(const hashtable<Key,Hash,KeyEqual,Allocator>& lhs,
                              const hashtable<Key,Hash,KeyEqual,Allocator>& rhs) -> bool {
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

NAMESPACE_CIEL_END

namespace std {

template<class Key, class Hash, class KeyEqual, class Alloc>
auto swap(ciel::hashtable<Key, Hash, KeyEqual, Alloc>& lhs,
          ciel::hashtable<Key, Hash, KeyEqual, Alloc>& rhs) noexcept(noexcept(lhs.swap(rhs))) -> void {
    lhs.swap(rhs);
}

}   // namespace std

#endif // CIELUTILS_INCLUDE_CIEL_HASHTABLE_HPP_