#ifndef CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_UNIQUE_PTR_HPP_
#define CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_UNIQUE_PTR_HPP_

#include <ciel/config.hpp>
#include <ciel/functional_impl/less.hpp>
#include <ciel/memory_impl/allocator_traits.hpp>
#include <ciel/memory_impl/default_delete.hpp>
#include <ciel/type_traits_impl/common_type.hpp>
#include <ciel/type_traits_impl/is_bounded_array.hpp>
#include <ciel/type_traits_impl/is_copy_constructible.hpp>
#include <ciel/type_traits_impl/is_default_constructible.hpp>
#include <ciel/type_traits_impl/is_move_assignable.hpp>
#include <ciel/type_traits_impl/is_move_constructible.hpp>
#include <ciel/type_traits_impl/is_pointer.hpp>
#include <ciel/type_traits_impl/is_reference.hpp>
#include <ciel/type_traits_impl/is_swappable.hpp>
#include <ciel/type_traits_impl/is_unbounded_array.hpp>
#include <ciel/type_traits_impl/remove_cvref.hpp>
#include <ciel/type_traits_impl/remove_pointer.hpp>

NAMESPACE_CIEL_BEGIN

namespace details {

template<class Deleter>
struct unique_ptr_constructor_helper {
    static_assert(!is_reference_v<Deleter>, "Deleter of ciel::unique_ptr can't be rvalue_reference");

    inline static constexpr int tag = 0;
};

template<class Deleter>
struct unique_ptr_constructor_helper<Deleter&> {
    inline static constexpr int tag = 1;
};

template<class Deleter>
struct unique_ptr_constructor_helper<const Deleter&> {
    inline static constexpr int tag = 2;
};

}   // namespace details

template<class T, class Deleter = default_delete<T>>
class unique_ptr {
public:
    using element_type = T;
    using deleter_type = Deleter;
    using pointer      = typename details::has_pointer<element_type, deleter_type>::type;

private:
    pointer ptr_;
    [[no_unique_address]] deleter_type dlt_;

public:
    constexpr unique_ptr() noexcept
        requires (is_default_constructible_v<deleter_type> && !is_pointer_v<deleter_type>) : ptr_(nullptr), dlt_() {}

    constexpr unique_ptr(std::nullptr_t) noexcept
        requires (is_default_constructible_v<deleter_type> && !is_pointer_v<deleter_type>) : ptr_(nullptr), dlt_() {}

    explicit unique_ptr(pointer p) noexcept
        requires (is_default_constructible_v<deleter_type> && !is_pointer_v<deleter_type>) : ptr_(p), dlt_() {}

    // case 1
    template<class A = remove_cvref_t<deleter_type>>
        requires is_nothrow_copy_constructible_v<deleter_type> &&
            is_constructible_v<deleter_type, const A&> &&
                (details::unique_ptr_constructor_helper<deleter_type>::tag == 0)
    unique_ptr(pointer p, const A& d) noexcept : ptr_(p), dlt_(std::forward<decltype(d)>(d)) {}

    template<class A = remove_cvref_t<deleter_type>>
        requires is_nothrow_move_constructible_v<deleter_type> &&
            is_constructible_v<deleter_type, A&&> &&
                (details::unique_ptr_constructor_helper<deleter_type>::tag == 0)
    unique_ptr(pointer p, A&& d) noexcept : ptr_(p), dlt_(std::forward<decltype(d)>(d)) {}

    // case 2
    template<class A = remove_cvref_t<deleter_type>>
        requires is_constructible_v<deleter_type, A&> &&
            (details::unique_ptr_constructor_helper<deleter_type>::tag == 1)
    unique_ptr(pointer p, A& d) noexcept : ptr_(p), dlt_(std::forward<decltype(d)>(d)) {}

    template<class A = remove_cvref_t<deleter_type>>
        requires (details::unique_ptr_constructor_helper<deleter_type>::tag == 1)
    unique_ptr(pointer p, A&& d) = delete;

    // case 3
    template<class A = remove_cvref_t<deleter_type>>
        requires is_constructible_v<deleter_type, const A&> &&
            (details::unique_ptr_constructor_helper<deleter_type>::tag == 2)
    unique_ptr(pointer p, const A& d) noexcept : ptr_(p), dlt_(std::forward<decltype(d)>(d)) {}

    template<class A = remove_cvref_t<deleter_type>>
        requires (details::unique_ptr_constructor_helper<deleter_type>::tag == 2)
    unique_ptr(pointer p, const A&& d) = delete;

    unique_ptr(unique_ptr&& u) noexcept requires is_move_constructible_v<deleter_type>
        : ptr_(u.release()), dlt_(std::forward<deleter_type>(u.get_deleter())) {}

    template<class U, class E>
        requires(is_convertible_v<typename unique_ptr<U, E>::pointer, pointer> &&
            !is_array_v<U> && ((is_reference_v<deleter_type> && is_same_v<deleter_type, E>) ||
            (!is_reference_v<deleter_type> && is_convertible_v<E, deleter_type>)))
    unique_ptr(unique_ptr<U, E>&& u) noexcept : ptr_(u.release()), dlt_(std::forward<deleter_type>(u.get_deleter())) {}

    unique_ptr(const unique_ptr&) = delete;
    auto operator=(const unique_ptr&) -> unique_ptr& = delete;

    ~unique_ptr() {
        reset();
    }

    auto operator=(unique_ptr&& r) noexcept -> unique_ptr& requires is_move_assignable_v<deleter_type> {
        reset(r.release());
        dlt_ = std::forward<deleter_type>(r.get_deleter());
        return *this;
    }

    template<class U, class E>
        requires (!is_array_v<U> &&
            is_convertible_v<typename unique_ptr<U, E>::pointer, pointer> &&
                is_assignable_v<deleter_type&, E&&>)
    auto operator=(unique_ptr<U, E>&& r) noexcept -> unique_ptr& {
        reset(r.release());
        dlt_ = std::forward<E>(r.get_deleter());
        return *this;
    }

    auto operator=(std::nullptr_t) noexcept -> unique_ptr& {
        reset();
        return *this;
    }

    [[nodiscard]] auto release() noexcept -> pointer {
        pointer res = ptr_;
        ptr_ = pointer();
        return res;
    }

    // Why there are three steps?
    // If pointer is smart pointer, and it throws when copy constructing or copy assigning,
    // ptr_ still holds pointer (It also needs pointer to be exception safe)
    //
    // So when old.reset(new); in a try block and it throws, old is still complete,
    // while new is supposed to be taken care of by users in catch block
    //
    // Also, if ptr_ hold the pointer that points to itself,
    // it can easily end up in infinite recursions of destructor and reset
    auto reset(pointer p = pointer()) noexcept -> void {
        pointer tmp = ptr_;
        ptr_ = p;
        if (tmp) {
            get_deleter()(tmp);
        }
    }

    auto swap(unique_ptr& other) noexcept -> void {
        using std::swap;
        swap(ptr_, other.ptr_);
        swap(dlt_, other.dlt_);
    }

    [[nodiscard]] auto get() const noexcept -> pointer {
        return ptr_;
    }

    [[nodiscard]] auto get_deleter() noexcept -> deleter_type& {
        return dlt_;
    }

    [[nodiscard]] auto get_deleter() const noexcept -> const deleter_type& {
        return dlt_;
    }

    [[nodiscard]] explicit operator bool() const noexcept {
        return get() != nullptr;
    }

    [[nodiscard]] auto operator*() const noexcept(noexcept(*declval<pointer>())) -> add_lvalue_reference_t<T> {
        return *get();
    }

    [[nodiscard]] auto operator->() const noexcept -> pointer {
        return get();
    }

};    // class unique_ptr

template<class T, class Deleter>
class unique_ptr<T[], Deleter> {
public:
    using element_type = T;
    using deleter_type = Deleter;
    using pointer      = typename details::has_pointer<element_type, deleter_type>::type;

private:
    pointer ptr_;
    [[no_unique_address]] deleter_type dlt_;

public:
    constexpr unique_ptr() noexcept
        requires (is_default_constructible_v<deleter_type> && !is_pointer_v<deleter_type>): ptr_(), dlt_() {}

    constexpr unique_ptr(std::nullptr_t) noexcept
        requires (is_default_constructible_v<deleter_type> && !is_pointer_v<deleter_type>): ptr_(), dlt_() {}

    template<class U>
        requires (is_default_constructible_v<deleter_type> && !is_pointer_v<deleter_type>)
    explicit unique_ptr(U p) noexcept : ptr_(p), dlt_() {}

    // case 1
    template<class U, class A = remove_cvref_t<deleter_type>>
        requires is_nothrow_copy_constructible_v<deleter_type> &&
            is_constructible_v<deleter_type, const A&> &&
                (details::unique_ptr_constructor_helper<deleter_type>::tag == 0)
    unique_ptr(U p, const A& d) noexcept : ptr_(p), dlt_(std::forward<decltype(d)>(d)) {}

    template<class U, class A = remove_cvref_t<deleter_type>>
        requires is_nothrow_move_constructible_v<deleter_type> &&
            is_constructible_v<deleter_type, A&&> &&
                (details::unique_ptr_constructor_helper<deleter_type>::tag == 0)
    unique_ptr(U p, A&& d) noexcept : ptr_(p), dlt_(std::forward<decltype(d)>(d)) {}

    // case 2
    template<class U, class A = remove_cvref_t<deleter_type>>
        requires is_constructible_v<deleter_type, A&> &&
            (details::unique_ptr_constructor_helper<deleter_type>::tag == 1)
    unique_ptr(U p, A& d) noexcept : ptr_(p), dlt_(std::forward<decltype(d)>(d)) {}

    template<class U, class A = remove_cvref_t<deleter_type>>
        requires (details::unique_ptr_constructor_helper<deleter_type>::tag == 1)
    unique_ptr(U p, A&& d) = delete;

    // case 3
    template<class U, class A = remove_cvref_t<deleter_type>>
        requires is_constructible_v<deleter_type, const A&> &&
            (details::unique_ptr_constructor_helper<deleter_type>::tag == 2)
    unique_ptr(U p, const A& d) noexcept : ptr_(p), dlt_(std::forward<decltype(d)>(d)) {}

    template<class U, class A = remove_cvref_t<deleter_type>>
        requires (details::unique_ptr_constructor_helper<deleter_type>::tag == 2)
    unique_ptr(U p, const A&& d) = delete;

    unique_ptr(unique_ptr&& u) noexcept requires is_move_constructible_v<deleter_type>
        : ptr_(u.release()), dlt_(std::forward<deleter_type>(u.get_deleter())) {}

    template<class U, class E>
        requires (is_array_v<U> &&
            is_same_v<pointer, element_type*> &&
                is_same_v<typename unique_ptr<U, E>::pointer, typename unique_ptr<U, E>::element_type*> &&
                    is_convertible_v<typename unique_ptr<U, E>::element_type(*)[], element_type(*)[]> &&
                        ((is_reference_v<deleter_type> && is_same_v<deleter_type, E>) ||
                        (!is_reference_v<deleter_type> && is_convertible_v<E, deleter_type>)))
    unique_ptr(unique_ptr<U, E>&& u) noexcept : ptr_(u.release()), dlt_(std::forward<deleter_type>(u.get_deleter())) {}

    unique_ptr(const unique_ptr&) = delete;
    auto operator=(const unique_ptr&) -> unique_ptr& = delete;

    ~unique_ptr() {
        reset();
    }

    auto operator=(unique_ptr&& r) noexcept -> unique_ptr& requires is_move_assignable_v<deleter_type> {
        reset(r.release());
        dlt_ = std::forward<deleter_type>(r.get_deleter());
        return *this;
    }

    template<class U, class E>
        requires (is_array_v<U>
            && is_same_v<pointer, element_type*>
            && is_same_v<typename unique_ptr<U, E>::pointer, typename unique_ptr<U, E>::element_type*>
            && is_convertible_v<typename unique_ptr<U, E>::element_type(*)[], element_type(*)[]>
            && is_assignable_v<deleter_type&, E&&>)
    auto operator=(unique_ptr<U, E>&& r) noexcept -> unique_ptr& {
        reset(r.release());
        dlt_ = std::forward<E>(r.get_deleter());
        return *this;
    }

    auto operator=(std::nullptr_t) noexcept -> unique_ptr& {
        reset();
        return *this;
    }

    [[nodiscard]] auto release() noexcept -> pointer {
        pointer res = ptr_;
        ptr_ = pointer();
        return res;
    }

    template<class U>
        requires (is_same_v<U, pointer> ||
            (is_same_v<pointer, element_type*> &&
                is_pointer_v<U> &&
                    is_convertible_v<remove_pointer_t<U>(*)[], element_type(*)[]>))
    auto reset(U p) noexcept -> void {
        pointer tmp = ptr_;
        ptr_ = p;
        if (tmp) {
            get_deleter()(tmp);
        }
    }

    auto reset(std::nullptr_t = nullptr) noexcept -> void {
        reset(pointer());
    }

    auto swap(unique_ptr& other) noexcept -> void {
        using std::swap;
        swap(ptr_, other.ptr_);
        swap(dlt_, other.dlt_);
    }

    [[nodiscard]] auto get() const noexcept -> pointer {
        return ptr_;
    }

    [[nodiscard]] auto get_deleter() noexcept -> deleter_type& {
        return dlt_;
    }

    [[nodiscard]] auto get_deleter() const noexcept -> const deleter_type& {
        return dlt_;
    }

    [[nodiscard]] explicit operator bool() const noexcept {
        return get() != nullptr;
    }

    [[nodiscard]] auto operator[](const size_t i) const -> T& {
        return get()[i];
    }

};    // class unique_ptr<T[], Deleter>

template<class T1, class D1, class T2, class D2>
[[nodiscard]] auto operator==(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) -> bool {
    return x.get() == y.get();
}

template<class T1, class D1, class T2, class D2>
[[nodiscard]] auto operator<(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) -> bool {
    return ciel::less<common_type_t<typename unique_ptr<T1, D1>::pointer,
                                    typename unique_ptr<T2, D2>::pointer>>()(x.get(), y.get());
}

template<class T1, class D1, class T2, class D2>
[[nodiscard]] auto operator<=(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) -> bool {
    return !(y < x);
}

template<class T1, class D1, class T2, class D2>
[[nodiscard]] auto operator>(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) -> bool {
    return y < x;
}

template<class T1, class D1, class T2, class D2>
[[nodiscard]] auto operator>=(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) -> bool {
    return !(x < y);
}

template<class T1, class D1, class T2, class D2>
    requires std::three_way_comparable_with<typename unique_ptr<T1, D1>::pointer, typename unique_ptr<T2, D2>::pointer>
[[nodiscard]] auto operator<=>(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y)
    -> std::compare_three_way_result_t<typename unique_ptr<T1, D1>::pointer, typename unique_ptr<T2, D2>::pointer> {
    return std::compare_three_way{}(x.get(), y.get());
}

template<class T, class D>
[[nodiscard]] auto operator==(const unique_ptr<T, D>& x, std::nullptr_t) noexcept -> bool {
    return !x;
}

template<class T, class D>
[[nodiscard]] auto operator<(const unique_ptr<T, D>& x, std::nullptr_t) -> bool {
    return ciel::less<typename unique_ptr<T, D>::pointer>()(x.get(), nullptr);
}

template<class T, class D>
[[nodiscard]] auto operator<(std::nullptr_t, const unique_ptr<T, D>& y) -> bool {
    return ciel::less<typename unique_ptr<T, D>::pointer>()(nullptr, y.get());
}

template<class T, class D>
[[nodiscard]] auto operator<=(const unique_ptr<T, D>& x, std::nullptr_t) -> bool {
    return !(nullptr < x);
}

template<class T, class D>
[[nodiscard]] auto operator<=(std::nullptr_t, const unique_ptr<T, D>& y) -> bool {
    return !(y < nullptr);
}

template<class T, class D>
[[nodiscard]] auto operator>(const unique_ptr<T, D>& x, std::nullptr_t) -> bool {
    return nullptr < x;
}

template<class T, class D>
[[nodiscard]] auto operator>(std::nullptr_t, const unique_ptr<T, D>& y) -> bool {
    return y < nullptr;
}

template<class T, class D>
[[nodiscard]] auto operator>=(const unique_ptr<T, D>& x, std::nullptr_t) -> bool {
    return !(x < nullptr);
}

template<class T, class D>
[[nodiscard]] auto operator>=(std::nullptr_t, const unique_ptr<T, D>& y) -> bool {
    return !(nullptr < y);
}

template<class T, class D>
    requires std::three_way_comparable<typename unique_ptr<T, D>::pointer>
[[nodiscard]] auto operator<=>(const unique_ptr<T, D>& x, std::nullptr_t)
    -> std::compare_three_way_result_t<typename unique_ptr<T, D>::pointer> {
    return std::compare_three_way{}(x.get(), static_cast<typename unique_ptr<T, D>::pointer>(nullptr));
}

template<class T, class... Args>
    requires (!is_array_v<T>)
[[nodiscard]] auto make_unique(Args&&... args) -> unique_ptr<T> {
    return unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template<class T>
    requires is_unbounded_array_v<T>
[[nodiscard]] auto make_unique(const size_t size) -> unique_ptr<T> {
    return unique_ptr<T>(new remove_extent_t<T>[size]());
}

// It's enough to use unbounded version,
// and if it's not deleted, unique_ptr<T[N]> will choose the main version, which is dumb
template<class T, class... Args>
    requires is_bounded_array_v<T>
auto make_unique(Args&&... args) = delete;

template<class T>
    requires (!is_array_v<T>)
[[nodiscard]] auto make_unique_for_overwrite() -> unique_ptr<T> {
    return unique_ptr<T>(new T);
}

template<class T>
    requires is_unbounded_array_v<T>
[[nodiscard]] auto make_unique_for_overwrite(const size_t size) -> unique_ptr<T> {
    return unique_ptr<T>(new remove_extent_t<T>[size]);
}

template<class T, class... Args>
auto make_unique_for_overwrite(Args&&... args) = delete;

template<class CharT, class Traits, class Y, class D>
auto operator<<(std::basic_ostream<CharT, Traits>& os, const unique_ptr<Y, D>& p)
    -> std::basic_ostream<CharT, Traits>& requires requires { os << p.get(); } {
    os << p.get();
    return os;
}

// serves for containers
template<class Alloc>
struct allocator_destructor {
private:
    using pointer = typename allocator_traits<Alloc>::pointer;
    Alloc alloc_;

public:
    explicit allocator_destructor(const Alloc& a) noexcept : alloc_(a) {}

    auto operator()(pointer p) noexcept -> void {
        allocator_traits<Alloc>::deallocate(alloc_, p, 1);
    }

};  // struct allocator_destructor

NAMESPACE_CIEL_END

namespace std {

template<class T, class D>
    requires ciel::is_swappable_v<D>
auto swap(ciel::unique_ptr<T, D>& lhs, ciel::unique_ptr<T, D>& rhs) noexcept -> void {
    lhs.swap(rhs);
}

}   // namespace std

#endif // CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_UNIQUE_PTR_HPP_