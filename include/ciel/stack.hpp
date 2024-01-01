#ifndef CIELUTILS_INCLUDE_CIEL_STACK_HPP_
#define CIELUTILS_INCLUDE_CIEL_STACK_HPP_

#include <ciel/config.hpp>
#include <ciel/deque.hpp>
#include <ciel/memory_impl/uses_allocator.hpp>

NAMESPACE_CIEL_BEGIN

// TODO: operator<=>

template<class T, class Container = deque<T>>
class stack {
public:
    using container_type  = Container;
    using value_type      = typename container_type::value_type;
    using size_type       = typename container_type::size_type;
    using reference       = typename container_type::reference;
    using const_reference = typename container_type::const_reference;

private:
    container_type c_;

public:
    stack() : stack(container_type()) {}

    explicit stack(const container_type& cont) : c_(cont) {}

    explicit stack(container_type&& cont) : c_(std::move(cont)) {}

    stack(const stack& other) : c_(other.c_) {}

    stack(stack&& other) noexcept : c_(std::move(other.c_)) {}

    template<class Iter>
    stack(Iter first, Iter last) : c_(first, last) {}

    template<class Alloc>
        requires uses_allocator_v<container_type, Alloc>
    explicit stack(const Alloc& alloc) : c_(alloc) {}

    template<class Alloc>
        requires uses_allocator_v<container_type, Alloc>
    stack(const container_type& cont, const Alloc& alloc) : c_(cont, alloc) {}

    template<class Alloc>
        requires uses_allocator_v<container_type, Alloc>
    stack(container_type&& cont, const Alloc& alloc) : c_(std::move(cont), alloc) {}

    template<class Alloc>
        requires uses_allocator_v<container_type, Alloc>
    stack(const stack& other, const Alloc& alloc) : c_(other.c_, alloc) {}

    template<class Alloc>
        requires uses_allocator_v<container_type, Alloc>
    stack(stack&& other, const Alloc& alloc) : c_(std::move(other.c_), alloc) {}

    template<class Iter, class Alloc>
        requires uses_allocator_v<container_type, Alloc>
    stack(Iter first, Iter last, const Alloc& alloc) : c_(first, last, alloc) {}

    ~stack() = default;

    auto operator=(const stack& other) -> stack& = default;

    auto operator=(stack&& other) noexcept -> stack& = default;

    [[nodiscard]] auto top() -> reference {
        return c_.back();
    }

    [[nodiscard]] auto top() const -> const_reference {
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
        c_.pop_back();
    }

    auto swap(stack& other) noexcept(is_nothrow_swappable_v<Container>) -> void {
        using std::swap;
        swap(c_, other.c_);
    }

};    // class stack

template<class T, class Container, class Alloc>
struct uses_allocator<stack<T, Container>, Alloc> : uses_allocator<Container, Alloc>::type {};

template<class Container>
stack(Container) -> stack<typename Container::value_type, Container>;

template<class Iter>
stack(Iter, Iter) -> stack<typename iterator_traits<Iter>::value_type>;

template<class Container, class Alloc>
    requires uses_allocator_v<Container, Alloc>
stack(Container, Alloc) -> stack<typename Container::value_type, Container>;

template<class Iter, class Alloc>
stack(Iter, Iter, Alloc)
    -> stack<typename iterator_traits<Iter>::value_type, deque<typename iterator_traits<Iter>::value_type, Alloc>>;

NAMESPACE_CIEL_END

namespace std {

template<class T, class Container>
    requires ciel::is_swappable_v<Container>
auto swap(ciel::stack<T, Container>& lhs, ciel::stack<T, Container>& rhs) noexcept(noexcept(lhs.swap(rhs))) -> void {
    lhs.swap(rhs);
}

}   // namespace std

#endif // CIELUTILS_INCLUDE_CIEL_STACK_HPP_