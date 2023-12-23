#ifndef CIELUTILS_INCLUDE_CIEL_QUEUE_HPP_
#define CIELUTILS_INCLUDE_CIEL_QUEUE_HPP_

#include <ciel/algorithm_impl/make_heap.hpp>
#include <ciel/algorithm_impl/pop_heap.hpp>
#include <ciel/algorithm_impl/push_heap.hpp>
#include <ciel/config.hpp>
#include <ciel/deque.hpp>
#include <ciel/memory_impl/uses_allocator.hpp>
#include <ciel/vector.hpp>

NAMESPACE_CIEL_BEGIN

// TODO: operator<=>

template<class T, class Container = deque<T>>
class queue {

    static_assert(is_same_v<T, typename Container::value_type>);

public:
    using container_type  = Container;
    using value_type      = typename container_type::value_type;
    using size_type       = typename container_type::size_type;
    using reference       = typename container_type::reference;
    using const_reference = typename container_type::const_reference;

private:
    container_type c_;

public:
    queue() : queue(container_type()) {}

    explicit queue(const container_type& cont) : c_(cont) {}

    explicit queue(container_type&& cont) : c_(std::move(cont)) {}

    queue(const queue& other) : c_(other.c_) {}

    queue(queue&& other) noexcept : c_(std::move(other.c_)) {}

    template<legacy_input_iterator Iter>
    queue(Iter first, Iter last) : c_(first, last) {}

    template<class Alloc>
        requires uses_allocator_v<container_type, Alloc>
    explicit queue(const Alloc& alloc) : c_(alloc) {}

    template<class Alloc>
        requires uses_allocator_v<container_type, Alloc>
    queue(const container_type& cont, const Alloc& alloc) : c_(cont, alloc) {}

    template<class Alloc>
        requires uses_allocator_v<container_type, Alloc>
    queue(container_type&& cont, const Alloc& alloc) : c_(std::move(cont), alloc) {}

    template<class Alloc>
        requires uses_allocator_v<container_type, Alloc>
    queue(const queue& other, const Alloc& alloc) : c_(other.c_, alloc) {}

    template<class Alloc>
        requires uses_allocator_v<container_type, Alloc>
    queue(queue&& other, const Alloc& alloc) : c_(std::move(other.c_), alloc) {}

    template<legacy_input_iterator Iter, class Alloc>
        requires uses_allocator_v<container_type, Alloc>
    queue(Iter first, Iter last, const Alloc& alloc) : c_(first, last, alloc) {}

    ~queue() = default;

    auto operator=(const queue& other) -> queue& = default;

    auto operator=(queue&& other) noexcept -> queue& = default;

    [[nodiscard]] auto front() -> reference {
        return c_.front();
    }

    [[nodiscard]] auto front() const -> const_reference {
        return c_.front();
    }

    [[nodiscard]] auto back() -> reference {
        return c_.back();
    }

    [[nodiscard]] auto back() const -> const_reference {
        return c_.back();
    }

    [[nodiscard]] auto empty() const -> bool {
        return c_.empty();
    }

    [[nodiscard]] auto size() const -> size_type {
        return c_.size();
    }

    auto push(const value_type& value) -> void {
        c_.push_back(value);
    }

    auto push(value_type&& value) -> void {
        c_.push_back(std::move(value));
    }

    template<class... Args>
    auto emplace(Args&& ... args) -> decltype(auto) {
        return c_.emplace_back(std::forward<Args>(args)...);
    }

    auto pop() -> void {
        c_.pop_front();
    }

    auto swap(queue& other) noexcept(is_nothrow_swappable_v<container_type>) -> void {
        using std::swap;
        swap(c_, other.c_);
    }

    template<class U, class C>
    [[nodiscard]] friend auto operator==(const queue<U, C>& lhs, const queue<U, C>& rhs) -> bool {
        return lhs.c_ == rhs.c_;
    }

    template<class U, class C>
    [[nodiscard]] friend auto operator!=(const queue<U, C>& lhs, const queue<U, C>& rhs) -> bool {
        return lhs.c_ != rhs.c_;
    }

    template<class U, class C>
    [[nodiscard]] friend auto operator<(const queue<U, C>& lhs, const queue<U, C>& rhs) -> bool {
        return lhs.c_ < rhs.c_;
    }

    template<class U, class C>
    [[nodiscard]] friend auto operator<=(const queue<U, C>& lhs, const queue<U, C>& rhs) -> bool {
        return lhs.c_ <= rhs.c_;
    }

    template<class U, class C>
    [[nodiscard]] friend auto operator>(const queue<U, C>& lhs, const queue<U, C>& rhs) -> bool {
        return lhs.c_ > rhs.c_;
    }

    template<class U, class C>
    [[nodiscard]] friend auto operator>=(const queue<U, C>& lhs, const queue<U, C>& rhs) -> bool {
        return lhs.c_ >= rhs.c_;
    }

};    // class queue

template<class T, class Container, class Alloc>
struct uses_allocator<queue<T, Container>, Alloc> : uses_allocator<Container, Alloc>::type {};

template<class Container>
queue(Container) -> queue<typename Container::value_type, Container>;

template<legacy_input_iterator Iter>
queue(Iter, Iter) -> queue<typename iterator_traits<Iter>::value_type>;

template<class Container, class Alloc>
    requires uses_allocator_v<Container, Alloc>
queue(Container, Alloc) -> queue<typename Container::value_type, Container>;

template<legacy_input_iterator Iter, class Alloc>
queue(Iter, Iter, Alloc)
    -> queue<typename iterator_traits<Iter>::value_type, deque<typename iterator_traits<Iter>::value_type, Alloc>>;

template<class T, class Container = vector<T>, class Compare = less<typename Container::value_type>>
    requires legacy_random_access_iterator<typename Container::iterator>
class priority_queue {

    static_assert(is_same_v<T, typename Container::value_type>);

public:
    using container_type  = Container;
    using value_compare   = Compare;
    using value_type      = typename container_type::value_type;
    using size_type       = typename container_type::size_type;
    using reference       = typename container_type::reference;
    using const_reference = typename container_type::const_reference;

private:
    container_type c_;
    [[no_unique_address]] value_compare comp_;

public:
    priority_queue()
        : priority_queue(value_compare(), container_type()) {}

    explicit priority_queue(const value_compare& compare)
        : priority_queue(compare, container_type()) {}

    priority_queue(const value_compare& compare, const container_type& cont)
        : c_(cont), comp_(compare) {
        ciel::make_heap(c_.begin(), c_.end(), comp_);
    }

    priority_queue(const value_compare& compare, container_type&& cont)
        : c_(std::move(cont)), comp_(compare) {
        ciel::make_heap(c_.begin(), c_.end(), comp_);
    }

    priority_queue(const priority_queue& other) = default;

    priority_queue(priority_queue&& other) noexcept = default;

    template<legacy_input_iterator Iter>
    priority_queue(Iter first, Iter last, const value_compare& compare = value_compare())
        : c_(first, last), comp_(compare) {
        ciel::make_heap(c_.begin(), c_.end(), comp_);
    }

    template<legacy_input_iterator Iter>
    priority_queue(Iter first, Iter last, const value_compare& compare, const container_type& cont)
        : c_(cont), comp_(compare) {
        c_.insert(c_.end(), first, last);
        ciel::make_heap(c_.begin(), c_.end(), comp_);
    }

    template<legacy_input_iterator Iter>
    priority_queue(Iter first, Iter last, const value_compare& compare, container_type&& cont)
        : c_(std::move(cont)), comp_(compare) {
        c_.insert(c_.end(), first, last);
        ciel::make_heap(c_.begin(), c_.end(), comp_);
    }

    template<class Alloc>
        requires uses_allocator_v<container_type, Alloc>
    explicit priority_queue(const Alloc& alloc)
        : c_(alloc), comp_(value_compare()) {}

    template<class Alloc>
        requires uses_allocator_v<container_type, Alloc>
    priority_queue(const value_compare& compare, const Alloc& alloc)
        : c_(alloc), comp_(compare) {}

    template<class Alloc>
        requires uses_allocator_v<container_type, Alloc>
    priority_queue(const value_compare& compare, const container_type& cont, const Alloc& alloc)
        : c_(cont, alloc), comp_(compare) {
        ciel::make_heap(c_.begin(), c_.end(), comp_);
    }

    template<class Alloc>
        requires uses_allocator_v<container_type, Alloc>
    priority_queue(const value_compare& compare, container_type&& cont, const Alloc& alloc)
        : c_(std::move(cont), alloc), comp_(compare) {
        ciel::make_heap(c_.begin(), c_.end(), comp_);
    }

    template<class Alloc>
        requires uses_allocator_v<container_type, Alloc>
    priority_queue(const priority_queue& other, const Alloc& alloc)
        : c_(other.c_, alloc), comp_(other.comp_) {}

    template<class Alloc>
        requires uses_allocator_v<container_type, Alloc>
    priority_queue(priority_queue&& other, const Alloc& alloc)
        : c_(std::move(other.c_), alloc), comp_(std::move(other.comp_)) {}

    template<legacy_input_iterator Iter, class Alloc>
        requires uses_allocator_v<container_type, Alloc>
    priority_queue(Iter first, Iter last, const Alloc& alloc)
        : c_(alloc), comp_(value_compare()) {
        c_.insert(c_.end(), first, last);
        ciel::make_heap(c_.begin(), c_.end(), comp_);
    }

    template<legacy_input_iterator Iter, class Alloc>
        requires uses_allocator_v<container_type, Alloc>
    priority_queue(Iter first, Iter last, const value_compare& compare, const Alloc& alloc)
        : c_(alloc), comp_(compare) {
        c_.insert(c_.end(), first, last);
        ciel::make_heap(c_.begin(), c_.end(), comp_);
    }

    template<legacy_input_iterator Iter, class Alloc>
        requires uses_allocator_v<container_type, Alloc>
    priority_queue(Iter first, Iter last, const value_compare& compare, const container_type& cont, const Alloc& alloc)
        : c_(cont, alloc), comp_(compare) {
        c_.insert(c_.end(), first, last);
        ciel::make_heap(c_.begin(), c_.end(), comp_);
    }

    template<legacy_input_iterator Iter, class Alloc>
        requires uses_allocator_v<container_type, Alloc>
    priority_queue(Iter first, Iter last, const value_compare& compare, container_type&& cont, const Alloc& alloc)
        : c_(std::move(cont), alloc), comp_(compare) {
        c_.insert(c_.end(), first, last);
        ciel::make_heap(c_.begin(), c_.end(), comp_);
    }

    ~priority_queue() = default;

    auto operator=(const priority_queue& other) -> priority_queue& = default;

    auto operator=(priority_queue&& other) noexcept -> priority_queue& = default;

    [[nodiscard]] auto top() const -> const_reference {
        return c_.front();
    }

    [[nodiscard]] auto empty() const -> bool {
        return c_.empty();
    }

    [[nodiscard]] auto size() const -> size_type {
        return c_.size();
    }

    auto push(const value_type& value) -> void {
        c_.push_back(value);
        ciel::push_heap(c_.begin(), c_.end(), comp_);
    }

    auto push(value_type&& value) -> void {
        c_.push_back(std::move(value));
        ciel::push_heap(c_.begin(), c_.end(), comp_);
    }

    template<class... Args>
    auto emplace(Args&& ... args) -> void {
        c_.emplace_back(std::forward<Args>(args)...);
        ciel::push_heap(c_.begin(), c_.end(), comp_);
    }

    auto pop() -> void {
        ciel::pop_heap(c_.begin(), c_.end(), comp_);
        c_.pop_back();
    }

    auto swap(priority_queue& other)
        noexcept(is_nothrow_swappable_v<container_type> && is_nothrow_swappable_v<value_compare>) -> void {
        using std::swap;
        swap(c_, other.c_);
        swap(comp_, other.comp_);
    }

    [[nodiscard]] auto container() const -> const container_type& {
        return c_;
    }

};    // class priority_queue

template<class T, class Container, class Compare, class Alloc>
struct uses_allocator<priority_queue<T, Compare, Container>, Alloc> : uses_allocator<Container, Alloc>::type {};

template<class Comp, class Container>
priority_queue(Comp, Container) -> priority_queue<typename Container::value_type, Container, Comp>;

template<legacy_input_iterator Iter, class Comp = less<typename iterator_traits<Iter>::value_type>,
    class Container = vector<typename iterator_traits<Iter>::value_type>>
priority_queue(Iter, Iter, Comp = Comp(), Container = Container())
    -> priority_queue<typename iterator_traits<Iter>::value_type, Container, Comp>;

template<class Comp, class Container, class Alloc>
    requires uses_allocator_v<Container, Alloc>
priority_queue(Comp, Container, Alloc) -> priority_queue<typename Container::value_type, Container, Comp>;

template<legacy_input_iterator Iter, class Alloc>
priority_queue(Iter, Iter, Alloc)
    -> priority_queue<typename iterator_traits<Iter>::value_type,
        vector<typename iterator_traits<Iter>::value_type, Alloc>, less<typename iterator_traits<Iter>::value_type>>;

template<legacy_input_iterator Iter, class Comp, class Alloc>
priority_queue(Iter, Iter, Comp, Alloc)
    -> priority_queue<typename iterator_traits<Iter>::value_type,
        vector<typename iterator_traits<Iter>::value_type, Alloc>, Comp>;

template<legacy_input_iterator Iter, class Comp, class Container, class Alloc>
    requires uses_allocator_v<Container, Alloc>
priority_queue(Iter, Iter, Comp, Container, Alloc) -> priority_queue<typename Container::value_type, Container, Comp>;

NAMESPACE_CIEL_END

namespace std {

template<class T, class Container>
    requires ciel::is_swappable_v<Container>
auto swap(ciel::queue<T, Container>& lhs,
          ciel::queue<T, Container>& rhs) noexcept(noexcept(lhs.swap(rhs))) -> void {
    lhs.swap(rhs);
}

template<class T, class Container, class Compare>
    requires ciel::is_swappable_v<Container> && is_swappable_v<Compare>
auto swap(ciel::priority_queue<T, Container, Compare>& lhs,
          ciel::priority_queue<T, Container, Compare>& rhs) noexcept(noexcept(lhs.swap(rhs))) -> void {
    lhs.swap(rhs);
}

}   // namespace std

#endif // CIELUTILS_INCLUDE_CIEL_QUEUE_HPP_