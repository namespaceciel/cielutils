#ifndef CIELUTILS_INCLUDE_CIEL_VARIANT_HPP_
#define CIELUTILS_INCLUDE_CIEL_VARIANT_HPP_

#include <ciel/config.hpp>
#include <ciel/memory_impl/construct_at.hpp>
#include <ciel/memory_impl/destroy_at.hpp>
#include <ciel/type_traits_impl/add_pointer.hpp>
#include <ciel/type_traits_impl/aligned_union.hpp>
#include <ciel/type_traits_impl/enable_if.hpp>
#include <ciel/type_traits_impl/find_index_in_args.hpp>
#include <ciel/type_traits_impl/is_arithmetic.hpp>
#include <ciel/type_traits_impl/is_constructible.hpp>
#include <ciel/type_traits_impl/is_copy_assignable.hpp>
#include <ciel/type_traits_impl/is_copy_constructible.hpp>
#include <ciel/type_traits_impl/is_default_constructible.hpp>
#include <ciel/type_traits_impl/is_destructible.hpp>
#include <ciel/type_traits_impl/is_move_assignable.hpp>
#include <ciel/type_traits_impl/is_move_constructible.hpp>
#include <ciel/type_traits_impl/is_pointer.hpp>
#include <ciel/type_traits_impl/is_swappable.hpp>
#include <ciel/type_traits_impl/remove_cvref.hpp>
#include <ciel/utility_impl/in_place.hpp>
#include <cstddef>
#include <cstdlib>
#include <variant>  // std::bad_variant_access

NAMESPACE_CIEL_BEGIN

inline constexpr size_t variant_npos = -1;

struct monostate {};

[[nodiscard]] constexpr auto operator==(monostate /*unused*/, monostate /*unused*/) noexcept -> bool {
    return true;
}

[[nodiscard]] constexpr auto operator<=>(monostate /*unused*/, monostate /*unused*/) noexcept -> std::strong_ordering {
    return std::strong_ordering::equal;
}

namespace details {

template<bool IsTriviallyDestructible, class... Types>
class variant_storage;

template<class... Types>
class variant_storage<true, Types...> {
private:
    aligned_union_t<16, Types...> buffer_;

public:
    template<class T, class... Args>
    auto set_as(Args&&... args) -> void {
        static_assert(sizeof(aligned_union_t<16, Types...>) >= sizeof(T), "T is larger than local buffer size.");
        ciel::construct_at(static_cast<remove_reference_t<T>*>(static_cast<void*>(&buffer_)),
                           std::forward<Args>(args)...);
    }

    template<class T, class U, class... Args>
    auto set_as(std::initializer_list<U> il, Args&&... args) -> void {
        static_assert(sizeof(aligned_union_t<16, Types...>) >= sizeof(T), "T is larger than local buffer size.");
        ciel::construct_at(static_cast<remove_reference_t<T>*>(static_cast<void*>(&buffer_)),
                           il,
                           std::forward<Args>(args)...);
    }

    template<class Ptr>
        requires is_pointer_v<Ptr>
    [[nodiscard]] auto get_as() noexcept -> Ptr {
        return static_cast<Ptr>(static_cast<void*>(&buffer_));
    }

    template<class Ptr>
        requires is_pointer_v<Ptr>
    [[nodiscard]] auto get_as() const noexcept -> Ptr {
        return static_cast<const Ptr>(static_cast<void*>(&buffer_));
    }

    auto destroy() const noexcept -> void {}

};  // class variant_storage<true, Types...>

template<class... Types>
class variant_storage<false, Types...> {
private:
    enum class Action : uint8_t {
        Destroy, Copy, Move
    };

    using handle_func_ptr = void(*)(Action, void*, void*);

    aligned_union_t<16, Types...> buffer_;
    handle_func_ptr handler_{nullptr};

    template<class T>
    static auto handle(const Action ac, T* thisp, T* other) -> void {
        switch (ac) {
            case Action::Destroy :
                if constexpr (is_destructible_v<T>) {
                    ciel::destroy_at(thisp);
                }
                return;
            case Action::Copy :
                if constexpr (is_copy_constructible_v<T>) {
                    ciel::construct_at(thisp, *other);
                }
                return;
            case Action::Move :
                if constexpr (is_move_constructible_v<T>) {
                    ciel::construct_at(thisp, std::move(*other));
                }
                return;
            default:
                unreachable();
        }
    }

    // Copy / Move
    template<class VariantStorageT>
        requires is_same_v<variant_storage, remove_cvref_t<VariantStorageT>>
    auto operation(const Action ac, VariantStorageT&& other) -> void {
        operation(Action::Destroy);
        if (other.handler_) {
            handler_ = other.handler_;
            handler_(ac, &buffer_, &other.buffer_);
        }
    }

    // Destroy
    auto operation(const Action ac) noexcept -> void {
        if (handler_) {
            handler_(ac, &buffer_, nullptr);
        }
    }

public:
    variant_storage() = default;

    variant_storage(const variant_storage& other) {
        operation(Action::Copy, other);
    }

    variant_storage(variant_storage&& other) noexcept {
        operation(Action::Move, std::move(other));
    }

    ~variant_storage() {
        operation(Action::Destroy);
    }

    auto operator=(const variant_storage& other) -> variant_storage& {
        operation(Action::Copy, other);
        return *this;
    }

    auto operator=(variant_storage&& other) noexcept -> variant_storage& {
        operation(Action::Move, std::move(other));
        return *this;
    }

    template<class T, class... Args>
    auto set_as(Args&&... args) -> void {
        static_assert(sizeof(aligned_union_t<16, Types...>) >= sizeof(T), "T is larger than local buffer size.");
        ciel::construct_at(static_cast<remove_reference_t<T>*>(static_cast<void*>(&buffer_)),
                           std::forward<Args>(args)...);
        handler_ = reinterpret_cast<handle_func_ptr>(&handle<remove_reference_t<T>>);
    }

    template<class T, class U, class... Args>
    auto set_as(std::initializer_list<U> il, Args&&... args) -> void {
        static_assert(sizeof(aligned_union_t<16, Types...>) >= sizeof(T), "T is larger than local buffer size.");
        ciel::construct_at(static_cast<remove_reference_t<T>*>(static_cast<void*>(&buffer_)),
                           il,
                           std::forward<Args>(args)...);
        handler_ = reinterpret_cast<handle_func_ptr>(&handle<remove_reference_t<T>>);
    }

    template<class Ptr>
        requires is_pointer_v<Ptr>
    [[nodiscard]] auto get_as() noexcept -> Ptr {
        return static_cast<Ptr>(static_cast<void*>(&buffer_));
    }

    template<class Ptr>
        requires is_pointer_v<Ptr>
    [[nodiscard]] auto get_as() const noexcept -> Ptr {
        return static_cast<const Ptr>(static_cast<void*>(&buffer_));
    }

    auto destroy() noexcept -> void {
        operation(Action::Destroy);
    }

};  // class variant_storage<false, Types...>

}   // namespace details

template<class... Types>
class variant;

template<class T>
struct variant_size;

template<class... Types>
struct variant_size<variant<Types...>> : integral_constant<size_t, sizeof...(Types)> {};

template<class T>
struct variant_size<const T> : integral_constant<size_t, variant_size<T>::value> {};

template <class T>
inline constexpr size_t variant_size_v = variant_size<T>::value;

namespace details {

template<size_t I, class... Ts>
struct variant_alternative_helper;

template<size_t I, class Head, class... Tail>
struct variant_alternative_helper<I, Head, Tail...> {
    using type = typename variant_alternative_helper<I - 1, Tail...>::type;
};

template<class Head, class... Tail>
struct variant_alternative_helper<0, Head, Tail...> {
    using type = Head;
};

}   // namespace details

template<size_t I, class T>
struct variant_alternative;

template<size_t I, class... Types>
struct variant_alternative<I, variant<Types...>> {
    using type = typename details::variant_alternative_helper<I, Types...>::type;
};

template<size_t I, class T>
struct variant_alternative<I, const T> : add_const<typename variant_alternative<I, T>::type> {};

template<size_t I, class T>
using variant_alternative_t = typename variant_alternative<I, T>::type;

template<size_t I, class... Types, class T = variant_alternative_t<I, variant<Types...>>>
[[nodiscard]] constexpr auto get(variant<Types...>& v) -> T& {
    static_assert(I < sizeof...(Types), "get is ill-formed if I is not a valid index in the variant typelist.");
    if (I != v.index()) {
        THROW(std::bad_variant_access());
    }
    return *v.storage_.template get_as<T*>();
}

template<size_t I, class... Types, class T = variant_alternative_t<I, variant<Types...>>>
[[nodiscard]] constexpr auto get(variant<Types...>&& v) -> T&& {
    static_assert(I < sizeof...(Types), "get is ill-formed if I is not a valid index in the variant typelist.");
    if (I != v.index()) {
        THROW(std::bad_variant_access());
    }
    return std::move(*v.storage_.template get_as<T*>());
}

template<size_t I, class... Types, class T = variant_alternative_t<I, variant<Types...>>>
[[nodiscard]] constexpr auto get(const variant<Types...>& v) -> const T& {
    static_assert(I < sizeof...(Types), "get is ill-formed if I is not a valid index in the variant typelist.");
    if (I != v.index()) {
        THROW(std::bad_variant_access());
    }
    return *v.storage_.template get_as<T*>();
}

template<size_t I, class... Types, class T = variant_alternative_t<I, variant<Types...>>>
[[nodiscard]] constexpr auto get(const variant<Types...>&& v) -> const T&& {
    static_assert(I < sizeof...(Types), "get is ill-formed if I is not a valid index in the variant typelist.");
    if (I != v.index()) {
        THROW(std::bad_variant_access());
    }
    return std::move(*v.storage_.template get_as<T*>());
}

template<class T, class... Types>
[[nodiscard]] constexpr auto get(variant<Types...>& v) -> T& {
    constexpr size_t I = find_index_in_Args_v<0, T, Types...>;
    static_assert(I < sizeof...(Types), "get is ill-formed if I is not a valid index in the variant typelist.");
    return get<I>(v);
}

template<class T, class... Types>
[[nodiscard]] constexpr auto get(variant<Types...>&& v) -> T&& {
    constexpr size_t I = find_index_in_Args_v<0, T, Types...>;
    static_assert(I < sizeof...(Types), "get is ill-formed if I is not a valid index in the variant typelist.");
    return get<I>(std::move(v));
}

template<class T, class... Types>
[[nodiscard]] constexpr auto get(const variant<Types...>& v) -> const T& {
    constexpr size_t I = find_index_in_Args_v<0, T, Types...>;
    static_assert(I < sizeof...(Types), "get is ill-formed if I is not a valid index in the variant typelist.");
    return get<I>(v);
}

template<class T, class... Types>
[[nodiscard]] constexpr auto get(const variant<Types...>&& v) -> const T&& {
    constexpr size_t I = find_index_in_Args_v<0, T, Types...>;
    static_assert(I < sizeof...(Types), "get is ill-formed if I is not a valid index in the variant typelist.");
    return get<I>(std::move(v));
}

template<size_t I, class... Types, class TPtr = add_pointer_t<variant_alternative_t<I, variant<Types...>>>>
[[nodiscard]] constexpr auto get_if(variant<Types...>* pv) noexcept -> TPtr {
    static_assert(I < sizeof...(Types), "get_if is ill-formed if I is not a valid index in the variant typelist.");
    if (!pv || pv->index() != I) {
        return nullptr;
    }
    return pv->storage_.template get_as<TPtr>();
}

template<size_t I, class... Types, class TConstPtr = add_pointer_t<const variant_alternative_t<I, variant<Types...>>>>
[[nodiscard]] constexpr auto get_if(const variant<Types...>* pv) noexcept -> TConstPtr {
    static_assert(I < sizeof...(Types), "get_if is ill-formed if I is not a valid index in the variant typelist.");
    if (!pv || pv->index() != I) {
        return nullptr;
    }
    return pv->storage_.template get_as<TConstPtr>();
}

template<class T, class... Types>
[[nodiscard]] constexpr auto get_if(variant<Types...>* pv) noexcept -> add_pointer_t<T> {
    constexpr size_t I = find_index_in_Args_v<0, T, Types...>;
    return get_if<I>(pv);
}

template<class T, class... Types>
[[nodiscard]] constexpr auto get_if(const variant<Types...>* pv) noexcept -> add_pointer_t<const T> {
    constexpr size_t I = find_index_in_Args_v<0, T, Types...>;
    return get_if<I>(pv);
}

namespace details {

// serve for variant's converting constructor/assignment, choose the proper type
// https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0608r3.html

template<class...>
struct type_list {};

// Check backward from the last element, check if type is already in pack
template<class T, class... Ts>
struct unique_type_list {
    template<class... Args>
    static auto types(type_list<Args...>) -> enable_if_t<!(is_same_v<T, Args> || ...), type_list<T, Args...>>;

    template<class... Args>
    static auto types(type_list<Args...>) -> enable_if_t<(is_same_v<T, Args> || ...), type_list<Args...>>;

    using type = decltype(types(declval<typename unique_type_list<Ts...>::type>()));
};

template<class T>
struct unique_type_list<T> {
    using type = type_list<T>;
};

template<class... Ts>
using unique_type_list_t = typename unique_type_list<Ts...>::type;

template<class, class>
struct narrowing_check : false_type {};

template<class Source, class Dest>
    requires requires { Dest{declval<Source>()}; }    // brace doesn't allow narrowing convention
struct narrowing_check<Source, Dest> : true_type {};

template<class T>
struct overload {
    template<class U>
        requires (!is_arithmetic_v<T> || narrowing_check<U, T>::value)    // get rid of arithmetic narrowing options
    auto operator()(T, U&&) const -> T;
};

template<class>
struct overload_set_impl;

// inherit converting function of every type, use overload_set to choose the proper type
template<class... Ts>
struct overload_set_impl<type_list<Ts...>> : overload<Ts>... {
    using overload<Ts>::operator()...;
};

template<class... Ts>
struct overload_set : overload_set_impl<unique_type_list_t<Ts...>> {};

template<class T, class OverloadSet, class ResultT = decltype(declval<OverloadSet>()(declval<T>(), declval<T>()))>
struct overload_resolution {
    using type = ResultT;
};

template<class T, class OverloadSet>
using overload_resolution_t = typename overload_resolution<T, OverloadSet>::type;

}   // namespace details

template<class... Types>
class variant {
private:
    static_assert(sizeof...(Types) > 0, "variant must have at least 1 type.");
    static_assert(!(is_void_v<Types> || ...), "variant does not allow void as an alternative type.");
    static_assert(!(is_reference_v<Types> || ...), "variant does not allow reference as an alternative type.");
    static_assert(!(is_array_v<Types> || ...), "variant does not allow array as an alternative type.");

    template<size_t, class... Ts, class T>
    friend constexpr auto get(variant<Ts...>&) -> T&;
    template<size_t, class... Ts, class T>
    friend constexpr auto get(variant<Ts...>&&) -> T&&;
    template<size_t, class... Ts, class T>
    friend constexpr auto get(const variant<Ts...>&) -> const T&;
    template<size_t, class... Ts, class T>
    friend constexpr auto get(const variant<Ts...>&&) -> const T&&;

    template<size_t, class... Ts, class Ptr>
    friend constexpr auto get_if(variant<Ts...>*) noexcept -> Ptr;
    template<size_t, class... Ts, class Ptr>
    friend constexpr auto get_if(const variant<Ts...>*) noexcept -> Ptr;

    using variant_storage_t = details::variant_storage<(is_trivially_destructible_v<Types> && ...), Types...>;
    using T0 = variant_alternative_t<0, variant>;

    size_t index_;
    variant_storage_t storage_;

public:
    constexpr variant() noexcept(is_nothrow_default_constructible_v<T0>) requires (is_default_constructible_v<T0>)
        : index_(0) {
        storage_.template set_as<T0>();
    }

    constexpr variant(const variant& other) requires (is_copy_constructible_v<Types> && ...)
        : index_(other.index_), storage_(other.storage_) {}

    constexpr variant(variant&& other) noexcept((is_nothrow_move_constructible_v<Types> && ...)) requires (is_move_constructible_v<Types> && ...)
        : index_(other.index_), storage_(std::move(other.storage_)) {}

    template<class T, class Tj = details::overload_resolution_t<T, details::overload_set<Types...>>, size_t I = find_index_in_Args_v<0, Tj, Types...>>
        requires (!is_same_v<remove_cvref_t<T>, variant> && !is_inplace_index<T>::value && !is_inplace_type<T>::value && is_constructible_v<Tj, T>)
    constexpr variant(T&& t) noexcept(is_nothrow_constructible_v<Tj, T>)
        : index_(I) {
        storage_.template set_as<Tj>(std::forward<T>(t));
    }

    template<class T, class... Args>
        requires is_constructible_v<T, Args...>
    constexpr explicit variant(in_place_type_t<T>, Args&&... args)
        : index_(find_index_in_Args_v<0, T, Types...>) {
        storage_.template set_as<T>(std::forward<Args>(args)...);
    }

    template<class T, class U, class... Args>
        requires is_constructible_v<T, std::initializer_list<U>&, Args...>
    constexpr explicit variant(in_place_type_t<T>, std::initializer_list<U> il, Args&&... args)
        : index_(find_index_in_Args_v<0, T, Types...>) {
        storage_.template set_as<T>(il, std::forward<Args>(args)...);
    }

    template<size_t I, class... Args, class T = variant_alternative_t<I, variant>>
        requires is_constructible_v<T, Args...>
    constexpr explicit variant(in_place_index_t<I>, Args&&... args)
        : index_(I) {
        storage_.template set_as<T>(std::forward<Args>(args)...);
    }

    template<size_t I, class U, class... Args, class T = variant_alternative_t<I, variant>>
        requires is_constructible_v<T, std::initializer_list<U>&, Args...>
    constexpr explicit variant(in_place_index_t<I>, std::initializer_list<U> il, Args&&... args)
        : index_(I) {
        storage_.template set_as<T>(il, std::forward<Args>(args)...);
    }

    constexpr ~variant() = default;

    constexpr auto operator=(const variant& other) -> variant& requires ((is_copy_constructible_v<Types> && is_copy_assignable_v<Types>) && ...) {
        if (this != addressof(other)) {
            index_ = other.index_;
            storage_ = other.storage_;
        }
        return *this;
    }

    constexpr auto operator=(variant&& other) noexcept(((is_nothrow_move_constructible_v<Types> && is_nothrow_move_assignable_v<Types>) && ...)) -> variant&
        requires ((is_move_constructible_v<Types> && is_move_assignable_v<Types>) && ...) {
        if (this != addressof(other)) {
            index_ = other.index_;
            other.index_ =
            storage_ = std::move(other.storage_);
        }
        return *this;
    }

    template<class T, class Tj = details::overload_resolution_t<T, details::overload_set<Types...>>, size_t I = find_index_in_Args_v<0, Tj, Types...>>
        requires (!is_same_v<remove_cvref_t<T>, variant> && is_assignable_v<Tj&, T> && is_constructible_v<Tj, T>)
    constexpr auto operator=(T&& t) noexcept(is_nothrow_assignable_v<Tj&, T> && is_nothrow_constructible_v<Tj, T>) -> variant& {
        if (!valueless_by_exception()) {
            storage_.destroy();
        }
        index_ = I;
        storage_.template set_as<Tj>(std::forward<T>(t));
        return *this;
    }

    [[nodiscard]] constexpr auto index() const noexcept -> size_t {
        return index_;
    }

    [[nodiscard]] constexpr auto valueless_by_exception() const noexcept -> bool {
        return index_ == variant_npos;
    }

    template<class T, class... Args, size_t I = find_index_in_Args_v<0, T, Types...>>
        requires is_constructible_v<T, Args...>
    constexpr auto emplace(Args&&... args) -> T& {
        return emplace<I>(std::forward<Args>(args)...);
    }

    template<class T, class U, class... Args, size_t I = find_index_in_Args_v<0, T, Types...>>
        requires is_constructible_v<T, std::initializer_list<U>&, Args...>
    constexpr auto emplace(std::initializer_list<U> il, Args&&... args) -> T& {
        return emplace<I>(il, std::forward<Args>(args)...);
    }

    template<size_t I, class... Args, class T = variant_alternative_t<I, variant>>
        requires is_constructible_v<T, Args...>
    constexpr auto emplace(Args&&... args) -> T& {
        if (!valueless_by_exception()) {
            storage_.destroy();
        }
        index_ = I;
        storage_.template set_as<T>(std::forward<Args>(args)...);
        return *get<I>(*this);
    }

    template<size_t I, class U, class... Args, class T = variant_alternative_t<I, variant>>
        requires is_constructible_v<T, std::initializer_list<U>&, Args...>
    constexpr auto emplace(std::initializer_list<U> il, Args&&... args) -> T& {
        if (!valueless_by_exception()) {
            storage_.destroy();
        }
        index_ = I;
        storage_.template set_as<T>(il, std::forward<Args>(args)...);
        return *get<I>(*this);
    }

    constexpr auto swap(variant& other) noexcept(((is_nothrow_move_constructible_v<Types> && is_nothrow_swappable_v<Types>) && ...)) -> void {
        using std::swap;
        swap(index_, other.index_);
        swap(storage_, other.storage_);
    }

};  // class variant

template<class T, class... Types, size_t I = find_index_in_Args_v<0, T, Types...>>
[[nodiscard]] constexpr auto holds_alternative(const variant<Types...>& v) noexcept -> bool {
    return static_cast<bool>(I == v.index());
}

template<class... Types>
[[nodiscard]] constexpr auto operator==(const variant<Types...>& lhs, const variant<Types...>& rhs) -> bool {
    if (lhs.index() != rhs.index()) {
        return false;
    }
    if (lhs.valueless_by_exception()) {
        return true;
    }
    return *get_if<lhs.index()>(addressof(lhs)) == *get_if<lhs.index()>(addressof(rhs));
}

template<class... Types>
[[nodiscard]] constexpr auto operator!=(const variant<Types...>& lhs, const variant<Types...>& rhs) -> bool {
    if (lhs.index() != rhs.index()) {
        return true;
    }
    if (lhs.valueless_by_exception()) {
        return false;
    }
    return *get_if<lhs.index()>(addressof(lhs)) != *get_if<lhs.index()>(addressof(rhs));
}

template<class... Types>
[[nodiscard]] constexpr auto operator<(const variant<Types...>& lhs, const variant<Types...>& rhs) -> bool {
    if (rhs.valueless_by_exception()) {
        return false;
    }
    if (lhs.valueless_by_exception()) {
        return true;
    }
    if (lhs.index < rhs.index()) {
        return true;
    }
    if (lhs.index() > rhs.index()) {
        return false;
    }
    return *get_if<lhs.index()>(addressof(lhs)) < *get_if<lhs.index()>(addressof(rhs));
}

template<class... Types>
[[nodiscard]] constexpr auto operator>(const variant<Types...>& lhs, const variant<Types...>& rhs) -> bool {
    if (lhs.valueless_by_exception()) {
        return false;
    }
    if (rhs.valueless_by_exception()) {
        return true;
    }
    if (lhs.index > rhs.index()) {
        return true;
    }
    if (lhs.index() < rhs.index()) {
        return false;
    }
    return *get_if<lhs.index()>(addressof(lhs)) > *get_if<lhs.index()>(addressof(rhs));
}

template<class... Types>
[[nodiscard]] constexpr auto operator<=(const variant<Types...>& lhs, const variant<Types...>& rhs) -> bool {
    if (lhs.valueless_by_exception()) {
        return true;
    }
    if (rhs.valueless_by_exception()) {
        return false;
    }
    if (lhs.index < rhs.index()) {
        return true;
    }
    if (lhs.index() > rhs.index()) {
        return false;
    }
    return *get_if<lhs.index()>(addressof(lhs)) <= *get_if<lhs.index()>(addressof(rhs));
}

template<class... Types>
[[nodiscard]] constexpr auto operator>=(const variant<Types...>& lhs, const variant<Types...>& rhs) -> bool {
    if (rhs.valueless_by_exception()) {
        return true;
    }
    if (lhs.valueless_by_exception()) {
        return false;
    }
    if (lhs.index > rhs.index()) {
        return true;
    }
    if (lhs.index() < rhs.index()) {
        return false;
    }
    return *get_if<lhs.index()>(addressof(lhs)) >= *get_if<lhs.index()>(addressof(rhs));
}

template<class... Types>
[[nodiscard]] constexpr auto operator<=>(const variant<Types...>& lhs, const variant<Types...>& rhs) -> std::common_comparison_category_t<std::compare_three_way_result_t<Types>...> {
    if (lhs.valueless_by_exception()) {
        if (rhs.valueless_by_exception()) {
            return std::strong_ordering::equal;
        }
        return std::strong_ordering::less;
    }
    if (rhs.valueless_by_exception()) {
        return std::strong_ordering::greater;
    }
    if (lhs.index() != rhs.index()) {
        return lhs.index() <=> rhs.index();
    }
    return *get_if<lhs.index()>(addressof(lhs)) <=> *get_if<lhs.index()>(addressof(rhs));
}

// TODO: visit

NAMESPACE_CIEL_END

namespace std {

template<class... Types>
    requires ((is_move_constructible_v<Types> && is_swappable_v<Types>) && ...)
constexpr auto swap(ciel::variant<Types...>& lhs,
                    ciel::variant<Types...>& rhs) noexcept(noexcept(lhs.swap(rhs))) -> void {
    lhs.swap(rhs);
}

template<>
struct hash<ciel::monostate> {
    auto operator()(ciel::monostate /*unused*/) const noexcept -> size_t {
        return 0x74159;
    }
};

}   // namespace std

#endif // CIELUTILS_INCLUDE_CIEL_VARIANT_HPP_