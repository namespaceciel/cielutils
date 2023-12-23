#ifndef CIELUTILS_INCLUDE_CIEL_AVL_H
#define CIELUTILS_INCLUDE_CIEL_AVL_H

#include <ciel/algorithm_impl/equal.hpp>
#include <ciel/algorithm_impl/is_sorted.hpp>
#include <ciel/algorithm_impl/max.hpp>
#include <ciel/config.hpp>
#include <ciel/iterator_impl/iterator_tag.hpp>
#include <ciel/iterator_impl/legacy_input_iterator.hpp>
#include <ciel/iterator_impl/reverse_iterator.hpp>
#include <ciel/memory_impl/allocator_traits.hpp>
#include <ciel/utility_impl/pair.hpp>
#include <cstddef>

NAMESPACE_CIEL_BEGIN

// To make the tree root non-special, we have a dummy_root that only have a left pointer pointing to real root,
// and the real root's parent pointer pointing to dummy_root.
// So dummy_root is only an avl_node_base, and common node derives from it and add other components
//
// And dummy_root is our end(), which makes iterator operations simple:
// increment: if right is not nullptr, goes to right and loops to the very left,
//            else it loops to parent until it's left child itself, and this parent is our target.
//            So the max node will follow this intruction and go to dummy_root/end(), which is great
// The same to decrement

struct avl_node_base {
    avl_node_base* left_;

    explicit avl_node_base(avl_node_base* l) noexcept : left_(l) {}

};  // struct avl_node_base

template<class T>
struct avl_node : avl_node_base {
    avl_node_base* right_;
    avl_node_base* parent_;
    size_t height_;
    T value_;

    template<class... Args>
    avl_node(avl_node_base* l, avl_node_base* r, avl_node_base* p, const size_t h, Args... args)
        : avl_node_base(l), right_(r), parent_(p), height_(h), value_(std::forward<Args>(args)...) {}

    [[nodiscard]] auto is_left_child() const noexcept -> bool {
        return parent_->left_ == this;
    }

    [[nodiscard]] static auto height(avl_node* node) noexcept -> size_t {
        if (!node) {
            return 0;
        }
        return node->height_;
    }

    auto height_adjust() noexcept -> void {
        height_ = 1 + ciel::max(height(static_cast<avl_node*>(left_)), height(static_cast<avl_node*>(right_)));
    }

};  // struct avl_node

template<class T, class Pointer, class Reference>
class avl_iterator {
public:
    using difference_type   = ptrdiff_t;
    using value_type        = T;
    using pointer           = Pointer;
    using reference         = Reference;
    using iterator_category = bidirectional_iterator_tag;
    using iterator_concept  = bidirectional_iterator_tag;

private:
    using base_node_type    = avl_node_base;
    using node_type         = avl_node<value_type>;

    base_node_type* it_;

public:
    [[nodiscard]] auto is_left_child() const noexcept -> bool {
        return static_cast<node_type*>(it_)->is_left_child();
    }

    auto goto_tree_min() noexcept -> void {
        while (it_->left_ != nullptr) {
            it_ = it_->left_;
        }
    }

    auto goto_tree_max() noexcept -> void {
        while (right()) {
            it_ = right();
        }
    }

    avl_iterator() noexcept : it_(nullptr) {}

    explicit avl_iterator(const base_node_type* p) noexcept : it_(const_cast<base_node_type*>(p)) {}

    avl_iterator(const avl_iterator&) noexcept = default;
    avl_iterator(avl_iterator&&) noexcept = default;

    template<class P, class R>
    avl_iterator(const avl_iterator<T, P, R>& other) noexcept : it_(const_cast<base_node_type*>(other.base())) {}

    ~avl_iterator() = default;

    auto operator=(const avl_iterator&) noexcept -> avl_iterator& = default;
    auto operator=(avl_iterator&&) noexcept -> avl_iterator& = default;

    [[nodiscard]] auto next() const noexcept -> avl_iterator {
        avl_iterator res(it_);
        ++res;
        return res;
    }

    [[nodiscard]] auto prev() const noexcept -> avl_iterator {
        avl_iterator res(it_);
        --res;
        return res;
    }

    [[nodiscard]] auto operator*() const noexcept -> reference {
        return static_cast<node_type*>(it_)->value_;
    }

    [[nodiscard]] auto operator->() const noexcept -> pointer {
        return &static_cast<node_type*>(it_)->value_;
    }

    auto operator++() noexcept -> avl_iterator& {
        if (right()) {
            it_ = right();
            goto_tree_min();
        } else {
            while (!is_left_child()) {
                it_ = parent();
            }
            it_ = parent();
        }
        return *this;
    }

    [[nodiscard]] auto operator++(int) noexcept -> avl_iterator {
        avl_iterator res(it_);
        ++*this;
        return res;
    }

    auto operator--() noexcept -> avl_iterator& {
        if (left()) {
            it_ = left();
            goto_tree_max();
        } else {
            while (is_left_child()) {
                it_ = parent();
            }
            it_ = parent();
        }
        return *this;
    }

    [[nodiscard]] auto operator--(int) noexcept -> avl_iterator {
        avl_iterator res(it_);
        --*this;
        return res;
    }

    [[nodiscard]] auto base() const noexcept -> base_node_type* {
        return it_;
    }

    [[nodiscard]] auto left() const noexcept -> base_node_type* {
        return it_->left_;
    }

    [[nodiscard]] auto right() const noexcept -> base_node_type* {
        return static_cast<node_type*>(it_)->right_;
    }

    [[nodiscard]] auto parent() const noexcept -> base_node_type* {
        return static_cast<node_type*>(it_)->parent_;
    }

    [[nodiscard]] explicit operator bool() const noexcept {
        return it_ != nullptr;
    }

};  // class avl_iterator

template<class T, class Pointer1, class Pointer2, class Reference1, class Reference2>
[[nodiscard]] auto operator==(const avl_iterator<T, Pointer1, Reference1>& lhs,
                              const avl_iterator<T, Pointer2, Reference2>& rhs) noexcept -> bool {
    return lhs.base() == rhs.base();
}

template<class T, class Pointer1, class Pointer2, class Reference1, class Reference2>
[[nodiscard]] auto operator!=(const avl_iterator<T, Pointer1, Reference1>& lhs,
                              const avl_iterator<T, Pointer2, Reference2>& rhs) noexcept -> bool {
    return !(lhs == rhs);
}

template<class T, class Compare, class Allocator>
class avl {
public:
    using value_type             = T;
    using value_compare          = Compare;
    using size_type              = size_t;
    using difference_type        = ptrdiff_t;
    using allocator_type         = Allocator;
    using reference              = value_type&;
    using const_reference        = const value_type&;
    using pointer                = typename allocator_traits<allocator_type>::pointer;
    using const_pointer          = typename allocator_traits<allocator_type>::const_pointer;
    using iterator               = avl_iterator<value_type, pointer, reference>;
    using const_iterator         = avl_iterator<value_type, const_pointer, const_reference>;
    using reverse_iterator       = ciel::reverse_iterator<iterator>;
    using const_reverse_iterator = ciel::reverse_iterator<const_iterator>;

private:
    using base_node_type         = avl_node_base;
    using node_type              = avl_node<value_type>;

    using alloc_traits           = allocator_traits<allocator_type>;
    using node_allocator         = typename alloc_traits::template rebind_alloc<node_type>;
    using node_alloc_traits      = typename alloc_traits::template rebind_traits<node_type>;

    base_node_type* start_;
    base_node_type end_node_;
    size_type size_;
    [[no_unique_address]] node_allocator allocator_;
    [[no_unique_address]] value_compare comp_;

    // depth: H, only used in clear
    auto destroy_and_deallocate_root(iterator root) noexcept -> void {
        if (root) {
            destroy_and_deallocate_root(iterator(root.left()));
            destroy_and_deallocate_root(iterator(root.right()));
            node_alloc_traits::destroy(allocator_, static_cast<node_type*>(root.base()));
            node_alloc_traits::deallocate(allocator_, static_cast<node_type*>(root.base()), 1);
        }
    }

    [[nodiscard]] auto height(node_type* node) const noexcept -> size_t {
        if (!node) {
            return 0;
        }
        return node->height_;
    }

    auto destroy_and_deallocate_node(iterator node) noexcept -> void {
        if (!node.parent()) {   // delete new node when there is duplicate node already
            node_alloc_traits::destroy(allocator_, static_cast<node_type*>(node.base()));
            node_alloc_traits::deallocate(allocator_, static_cast<node_type*>(node.base()), 1);
            --size_;
            return;
        }

        if (node == begin()) {
            start_ = node.next().base();
        }

        // upping records the first position that needs to recalculate height_,
        // when deletion is done, go up taking care of height_ and balance from upping.
        // Note that upping is always node.parent() except for node has both children
        base_node_type* upping{node.parent()};

        if (node.left()) {
            // If node has both children, find previous node to replace it. upping will update only in this situation
            if (node.right()) {
                auto* prev = static_cast<node_type*>(node.prev().base());
                // if prev is left child, then head.left() == prev and prev doesn't have right child
                if (prev->is_left_child()) {
                    upping = prev;

                    prev->right_ = node.right();
                    static_cast<node_type*>(node.right())->parent_ = prev;

                    if (node.is_left_child()) {
                        node.parent()->left_ = prev;
                    } else {
                        static_cast<node_type*>(node.parent())->right_ = prev;
                    }
                    prev->parent_ = node.parent();
                } else {
                    upping = prev->parent_;

                    static_cast<node_type*>(prev->parent_)->right_ = prev->left_;
                    if (prev->left_) {
                        static_cast<node_type*>(prev->left_)->parent_ = prev->parent_;
                    }

                    prev->left_ = node.left();
                    prev->right_ = node.right();
                    prev->parent_ = node.parent();

                    static_cast<node_type*>(prev->left_)->parent_ = prev;
                    static_cast<node_type*>(prev->right_)->parent_ = prev;

                    if (node.is_left_child()) {
                        prev->parent_->left_ = prev;
                    } else {
                        static_cast<node_type*>(prev->parent_)->right_ = prev;
                    }
                }
            } else {    // only has left child
                if (node.is_left_child()) {
                    node.parent()->left_ = node.left();
                } else {
                    static_cast<node_type*>(node.parent())->right_ = node.left();
                }
                static_cast<node_type*>(node.left())->parent_ = node.parent();
            }
        } else if (node.right()) {      // only has right child
            if (node.is_left_child()) {
                node.parent()->left_ = node.right();
            } else {
                static_cast<node_type*>(node.parent())->right_ = node.right();
            }
            static_cast<node_type*>(node.right())->parent_ = node.parent();
        } else {       // leaf node
            if (node.is_left_child()) {
                node.parent()->left_ = nullptr;
            } else {
                static_cast<node_type*>(node.parent())->right_ = nullptr;
            }
        }

        while (upping != addressof(end_node_)) {
            static_cast<node_type*>(upping)->height_adjust();

            if (const auto balance = static_cast<make_signed_t<size_t>>(height(static_cast<node_type*>(upping->left_)))
                    - static_cast<make_signed_t<size_t>>(height(static_cast<node_type*>(static_cast<node_type*>(upping)->right_)));
                balance > 1) {
                if (height(static_cast<node_type*>(upping->left_->left_)) >=
                    height(static_cast<node_type*>(static_cast<node_type*>(upping->left_)->right_))) {
                    right_rotate(static_cast<node_type*>(upping));
                } else {
                    left_rotate(static_cast<node_type*>(upping->left_));
                    right_rotate(static_cast<node_type*>(upping));
                }
                upping = static_cast<node_type*>(upping)->parent_;
            } else if (balance < -1) {
                if (height(static_cast<node_type*>(static_cast<node_type*>(static_cast<node_type*>(upping)->right_)->right_)) >=
                    height(static_cast<node_type*>(static_cast<node_type*>(upping)->right_->left_))) {
                    left_rotate(static_cast<node_type*>(upping));
                } else {
                    right_rotate(static_cast<node_type*>(static_cast<node_type*>(upping)->right_));
                    left_rotate(static_cast<node_type*>(upping));
                }
                upping = static_cast<node_type*>(upping)->parent_;
            }

            upping = static_cast<node_type*>(upping)->parent_;
        }

        node_alloc_traits::destroy(allocator_, static_cast<node_type*>(node.base()));
        node_alloc_traits::deallocate(allocator_, static_cast<node_type*>(node.base()), 1);
        --size_;
    }

    template<class Key>
    [[nodiscard]] auto is_right_insert_unique_place(iterator pos, const Key& key) const noexcept -> bool {
        return static_cast<bool>((pos == end() || comp_(key, *pos)) && (pos == begin() || comp_(*pos.prev(), key)));
    }

    template<class Key>
    [[nodiscard]] auto is_right_insert_multi_place(iterator pos, const Key& key) const noexcept -> bool {
        return static_cast<bool>((pos == end() || !comp_(*pos, key)) && (pos == begin() || !comp_(key, *pos.prev())));
    }

    auto right_rotate(node_type* head) noexcept -> void {
        auto* new_head = static_cast<node_type*>(head->left_);
        head->left_ = new_head->right_;
        new_head->right_ = head;
        if (head->is_left_child()) {
            head->parent_->left_ = new_head;
        } else {
            static_cast<node_type*>(head->parent_)->right_= new_head;
        }

        new_head->parent_ = head->parent_;
        head->parent_ = new_head;
        if (head->left_) {
            static_cast<node_type*>(head->left_)->parent_ = head;
        }

        head->height_adjust();
        new_head->height_adjust();
    }

    auto left_rotate(node_type* head) noexcept -> void {
        auto* new_head = static_cast<node_type*>(head->right_);
        head->right_ = new_head->left_;
        new_head->left_ = head;
        if (head->is_left_child()) {
            head->parent_->left_ = new_head;
        } else {
            static_cast<node_type*>(head->parent_)->right_= new_head;
        }

        new_head->parent_ = head->parent_;
        head->parent_ = new_head;
        if (head->right_) {
            static_cast<node_type*>(head->right_)->parent_ = head;
        }

        head->height_adjust();
        new_head->height_adjust();
    }

    template<class... Args>
    [[nodiscard]] auto allocate_and_construct_node(Args&& ... args) -> node_type* {
        node_type* new_node = node_alloc_traits::allocate(allocator_, 1);
        CIEL_TRY {
            node_alloc_traits::construct(allocator_, new_node, nullptr, nullptr, nullptr, 1,
                                         std::forward<Args>(args)...);
        } CIEL_CATCH(...) {
            node_alloc_traits::deallocate(allocator_, new_node, 1);
            CIEL_THROW;
        }
        ++size_;
        return new_node;
    }

    // The place to insert is the previous to pos,
    // when insertion is done, go up taking care of height_ and balance from inserting place.
    auto insert_node_at(iterator pos, node_type* new_node) noexcept -> iterator {
        iterator res{[&] {
            if (pos.left()) {
                iterator l(pos.left());
                l.goto_tree_max();
                static_cast<node_type*>(l.base())->right_ = new_node;
                new_node->parent_ = l.base();
                return l.right();
            } else {
                pos.base()->left_ = new_node;
                new_node->parent_ = pos.base();
                if (pos.base() == start_) {
                    start_ = new_node;
                }
                return pos.left();
            }
        }()};
        auto* upping = static_cast<node_type*>(res.base());
        while (upping->parent_ != addressof(end_node_)) {
            upping = static_cast<node_type*>(upping->parent_);
            upping->height_adjust();

            // Following codes only happen once at most
            if (const auto balance = static_cast<make_signed_t<size_t>>(height(static_cast<node_type*>(upping->left_)))
                - static_cast<make_signed_t<size_t>>(height(static_cast<node_type*>(upping->right_)));
                                            balance > 1) {
                if (comp_(new_node->value_, static_cast<node_type*>(upping->left_)->value_)) {
                    right_rotate(upping);
                } else {
                    left_rotate(static_cast<node_type*>(upping->left_));
                    right_rotate(upping);
                }
                upping = static_cast<node_type*>(upping->parent_);
            } else if (balance < -1) {
                if (comp_(static_cast<node_type*>(upping->right_)->value_, new_node->value_)) {
                    left_rotate(upping);
                } else {
                    right_rotate(static_cast<node_type*>(upping->right_));
                    left_rotate(upping);
                }
                upping = static_cast<node_type*>(upping->parent_);
            }
        }
        return res;
    }

public:
    avl()
        : start_(&end_node_), end_node_(nullptr), size_(0), allocator_(), comp_() {}

    explicit avl(const value_compare& c, const allocator_type& alloc)
        : start_(&end_node_), end_node_(nullptr), size_(0), allocator_(alloc), comp_(c) {}

    explicit avl(const allocator_type& alloc)
        : start_(&end_node_), end_node_(nullptr), size_(0), allocator_(alloc), comp_() {}

    template<legacy_input_iterator Iter>
    avl(true_type /*unused*/, Iter first, Iter last, const value_compare& c, const allocator_type& alloc)
        : start_(&end_node_), end_node_(nullptr), size_(0), allocator_(alloc), comp_(c) {
        range_insert_multi(first, last);
    }

    template<legacy_input_iterator Iter>
    avl(false_type /*unused*/, Iter first, Iter last, const value_compare& c, const allocator_type& alloc)
        : start_(&end_node_), end_node_(nullptr), size_(0), allocator_(alloc), comp_(c) {
        range_insert_unique(first, last);
    }

    avl(const avl& other)
        : start_(&end_node_), end_node_(nullptr), size_(0),
          allocator_(node_alloc_traits::select_on_container_copy_construction(other.get_allocator())), comp_() {
        range_insert_multi(other.begin(), other.end());
    }

    avl(const avl& other, const allocator_type& alloc)
        : start_(&end_node_), end_node_(nullptr), size_(0), allocator_(alloc), comp_() {
        range_insert_multi(other.begin(), other.end());
    }

    avl(avl&& other) noexcept
        : start_(other.size_ ? other.start_ : &end_node_), end_node_(other.end_node_), size_(other.size_),
          allocator_(std::move(other.allocator_)), comp_(std::move(other.comp_)) {
        if (end_node_.left_ != nullptr) {
            static_cast<node_type*>(end_node_.left_)->parent_ = &end_node_;
        }
        other.start_ = &other.end_node_;
        other.end_node_.left_ = nullptr;
        other.size_ = 0;
    }

    avl(avl&& other, const allocator_type& alloc) noexcept
        : start_(other.size_ ? other.start_ : &end_node_), end_node_(other.end_node_), size_(other.size_),
          allocator_(alloc), comp_(std::move(other.comp_)) {
        if (end_node_.left_ != nullptr) {
            static_cast<node_type*>(end_node_.left_)->parent_ = &end_node_;
        }
        other.start_ = &other.end_node_;
        other.end_node_.left_ = nullptr;
        other.size_ = 0;
    }

    ~avl() {
        clear();
    }

    auto operator=(const avl& other) -> avl& {
        if (this == addressof(other)) {
            return *this;
        }
        if (alloc_traits::propagate_on_container_copy_assignment::value) {
            if (allocator_ != other.allocator_) {
                avl(other.allocator_).swap(*this);
                range_insert_multi(other.begin(), other.end());
                return *this;
            }
            allocator_ = other.allocator_;
        }
        clear();
        range_insert_multi(other.begin(), other.end());
        return *this;
    }

    auto operator=(avl&& other) noexcept -> avl& {
        if (this == addressof(other)) {
            return *this;
        }
        if (!alloc_traits::propagate_on_container_move_assignment::value && allocator_ != other.allocator_) {
            clear();
            range_insert_multi(other.begin(), other.end());
            return *this;
        }
        if (alloc_traits::propagate_on_container_move_assignment::value) {
            allocator_ = other.allocator_;
        }
        clear();
        start_ = other.size_ ? other.start_ : &end_node_;
        end_node_ = other.end_node_;
        if (end_node_.left_ != nullptr) {
            static_cast<node_type*>(end_node_.left_)->parent_ = &end_node_;
        }
        size_ = other.size_;
        other.start_ = &other.end_node_;
        other.end_node_.left_ = nullptr;
        other.size_ = 0;
        return *this;
    }

    [[nodiscard]] auto get_allocator() const noexcept -> allocator_type {
        return allocator_;
    }

    [[nodiscard]] auto begin() noexcept -> iterator {
        return iterator(start_);
    }

    [[nodiscard]] auto begin() const noexcept -> const_iterator {
        return const_iterator(start_);
    }

    [[nodiscard]] auto end() noexcept -> iterator {
        return iterator(&end_node_);
    }

    [[nodiscard]] auto end() const noexcept -> const_iterator {
        return const_iterator(&end_node_);
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
        destroy_and_deallocate_root(iterator(end().left()));
        end_node_.left_ = nullptr;
        start_ = &end_node_;
        size_ = 0;
    }

    // When [first, last) is ordered range, the complexity turn to H + N
    template<legacy_input_iterator Iter>
    auto range_insert_unique(Iter first, Iter last) -> void {
        if (is_forward_iterator<Iter>::value && ciel::is_sorted(first, last, comp_)) {
            iterator pos = lower_bound(*first);
            while (first != last) {
                while (!is_right_insert_multi_place(pos, *first)) {
                    ++pos;
                }
                if (is_right_insert_unique_place(pos, *first)) {
                    insert_node_at(pos, allocate_and_construct_node(*first));
                }
                ++first;
            }
        } else {
            while (first != last) {
                if (iterator pos = lower_bound(*first); is_right_insert_unique_place(pos, *first)) {
                    insert_node_at(pos, allocate_and_construct_node(*first));
                }
                ++first;
            }
        }
    }

    template<legacy_input_iterator Iter>
    auto range_insert_multi(Iter first, Iter last) -> void {
        if (is_forward_iterator<Iter>::value && ciel::is_sorted(first, last, comp_)) {
            iterator pos = lower_bound(*first);
            while (first != last) {
                while (!is_right_insert_multi_place(pos, *first)) {
                    ++pos;
                }
                insert_node_at(pos, allocate_and_construct_node(*first));
                ++first;
            }
        } else {
            while (first != last) {
                insert_node_at(lower_bound(*first), allocate_and_construct_node(*first));
                ++first;
            }
        }
    }

    template<class... Args>
    auto emplace_unique(Args&& ... args) -> pair<iterator, bool> {
        node_type* new_node = allocate_and_construct_node(std::forward<Args>(args)...);
        if (iterator hint = lower_bound(new_node->value_); is_right_insert_unique_place(hint, new_node->value_)) {
            return {insert_node_at(hint, new_node), true};
        } else {
            destroy_and_deallocate_node(iterator(new_node));
            return {hint, false};
        }
    }

    template<class... Args>
    auto emplace_multi(Args&& ... args) -> iterator {
        node_type* new_node = allocate_and_construct_node(std::forward<Args>(args)...);
        iterator hint = lower_bound(new_node->value_);
        return insert_node_at(hint, new_node);
    }

    template<class... Args>
    auto emplace_unique_hint(const_iterator hint, Args&& ... args) -> iterator {
        node_type* new_node = allocate_and_construct_node(std::forward<Args>(args)...);
        if (!is_right_insert_multi_place(hint, new_node->value_)) {
            hint = lower_bound(new_node->value_);
        }
        if (is_right_insert_unique_place(hint, new_node->value_)) {
            return insert_node_at(hint, new_node);
        } else {
            destroy_and_deallocate_node(iterator(new_node));
            return hint;
        }
    }

    template<class... Args>
    auto emplace_multi_hint(const_iterator hint, Args&& ... args) -> iterator {
        node_type* new_node = allocate_and_construct_node(std::forward<Args>(args)...);
        if (!is_right_insert_multi_place(hint, new_node->value_)) {
            hint = lower_bound(new_node->value_);
        }
        return insert_node_at(hint, new_node);
    }

    auto erase(iterator first, iterator last) noexcept -> iterator {
        while (first != last) {
            destroy_and_deallocate_node(first++);
        }
        return last;
    }

    template<class Key>
    auto erase(const Key& key) -> size_type {
        auto er = equal_range(key);
        const size_type res = size();
        erase(er.first, er.second);
        return res - size();
    }

    auto swap(avl& other) noexcept -> void {
        using std::swap;

        auto* temp_start = start_;
        swap(end_node_, other.end_node_);
        if (end_node_.left_ != nullptr) {
            static_cast<node_type*>(end_node_.left_)->parent_ = &end_node_;
            start_ = other.start_;
        } else {
            start_ = &end_node_;
        }
        if (other.end_node_.left_ != nullptr) {
            static_cast<node_type*>(other.end_node_.left_)->parent_ = &other.end_node_;
            other.start_ = temp_start;
        } else {
            other.start_ = &other.end_node_;
        }
        swap(size_, other.size_);
        swap(allocator_, other.allocator_);
        swap(comp_, other.comp_);
    }

    // TODO: extract, merge

    template<class Key>
    [[nodiscard]] auto count_unique(const Key& key) const noexcept -> size_type {
        return static_cast<size_type>(contains(key));
    }

    template<class Key>
    [[nodiscard]] auto count_multi(const Key& key) const noexcept -> size_type {
        size_type res = 0;
        iterator pos = find(key);
        if (pos != end()) {
            do {
                ++pos;
                ++res;
            } while (pos != end() && !comp_(key, *pos));
        }
        return res;
    }

    template<class Key>
    [[nodiscard]] auto find(const Key& key) const noexcept -> const_iterator {
        auto lb = lower_bound(key);
        if (lb != end() && !comp_(key, *lb)) {
            return lb;
        }
        return end();
    }

    template<class Key>
    [[nodiscard]] auto contains(const Key& key) const noexcept -> bool {
        return find(key) != end();
    }

    template<class Key>
    [[nodiscard]] auto equal_range(const Key& key) const noexcept -> pair<const_iterator, const_iterator> {
        return {lower_bound(key), upper_bound(key)};
    }

    template<class Key>
    [[nodiscard]] auto lower_bound(const Key& key) const noexcept -> const_iterator {
        iterator root(end().left());
        iterator res(end());
        while (root) {
            if (!comp_(*root, key)) {
                res = root;
                root = iterator(root.left());
            } else {
                root = iterator(root.right());
            }
        }
        return res;
    }

    template<class Key>
    [[nodiscard]] auto upper_bound(const Key& key) const noexcept -> const_iterator {
        iterator root(end().left());
        iterator res(end());
        while (root) {
            if (comp_(key, *root)) {
                res = root;
                root = iterator(root.left());
            } else {
                root = iterator(root.right());
            }
        }
        return res;
    }

    [[nodiscard]] auto value_comp() const -> value_compare {
        return comp_;
    }

};  // class avl

template<class Key, class Compare, class Alloc>
[[nodiscard]] auto operator==(const avl<Key, Compare, Alloc>& lhs, const avl<Key, Compare, Alloc>& rhs) noexcept
    -> bool {
    if (lhs.size() != rhs.size()) {
        return false;
    }
    return equal(lhs.begin(), lhs.end(), rhs.begin());
}

NAMESPACE_CIEL_END

namespace std {

template<class Key, class Compare, class Alloc>
auto swap(ciel::avl<Key, Compare, Alloc>& lhs,
          ciel::avl<Key, Compare, Alloc>& rhs) noexcept(noexcept(lhs.swap(rhs))) -> void {
    lhs.swap(rhs);
}

}   // namespace std

#endif // CIELUTILS_INCLUDE_CIEL_AVL_H