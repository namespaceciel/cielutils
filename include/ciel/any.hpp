#ifndef CIELUTILS_INCLUDE_CIEL_ANY_HPP_
#define CIELUTILS_INCLUDE_CIEL_ANY_HPP_

#include <any>  // std::bad_any_cast
#include <ciel/config.hpp>
#include <ciel/memory_impl/addressof.hpp>
#include <ciel/memory_impl/allocator.hpp>
#include <ciel/memory_impl/allocator_traits.hpp>
#include <ciel/memory_impl/construct_at.hpp>
#include <ciel/memory_impl/destroy_at.hpp>
#include <ciel/type_traits_impl/aligned_storage.hpp>
#include <ciel/type_traits_impl/conditional.hpp>
#include <ciel/type_traits_impl/decay.hpp>
#include <ciel/type_traits_impl/is_copy_constructible.hpp>
#include <ciel/type_traits_impl/is_same.hpp>
#include <ciel/utility_impl/in_place.hpp>
#include <cstdint>
#include <typeinfo>

NAMESPACE_CIEL_BEGIN

namespace details {

enum class Action : uint8_t {
    Destroy, Copy, Move, Get, TypeInfo
};

template<class> struct small_handler;
template<class> struct large_handler;

template<class T>
using handler_type = conditional_t<is_small_object<T>::value, small_handler<T>, large_handler<T>>;

}   // namespace details

class any {
private:
    using action_type     = details::Action;

    using handle_func_ptr = void* (*)(action_type, any const*, any*, const std::type_info*);

    union storage {
        constexpr storage() : ptr_(nullptr) {}
        void* ptr_;
        buffer_type buf_;
    };

    handle_func_ptr h_{nullptr};
    storage s_;

    auto call(const action_type a, any* other = nullptr, const std::type_info* info = nullptr) -> void* {
        return h_(a, this, other, info);
    }

    auto call(const action_type a, any* other = nullptr, const std::type_info* info = nullptr) const -> void* {
        return h_(a, this, other, info);
    }

    template<class> friend struct details::small_handler;
    template<class> friend struct details::large_handler;
    template<class T> friend auto any_cast(any* /*operand*/) noexcept -> T*;

public:
    constexpr any() noexcept = default;

    any(const any& other) {
        if (other.h_ != nullptr) {
            other.call(action_type::Copy, this);
        }
    }

    any(any&& other) noexcept {
        if (other.h_ != nullptr) {
            other.call(action_type::Move, this);
        }
    }

    template<class ValueType, class T = decay_t<ValueType>>
        requires (!is_same_v<T, any> && !is_inplace_type<T>::value && is_copy_constructible_v<T>)
    any(ValueType&& value) {
        details::handler_type<T>::create(*this, std::forward<ValueType>(value));
    }

    template<class ValueType, class... Args, class T = decay_t<ValueType>>
        requires (is_constructible_v<T, Args...> && is_copy_constructible_v<T>)
    explicit any(in_place_type_t<ValueType> /*unused*/, Args&& ... args) {
        details::handler_type<T>::create(*this, std::forward<Args>(args)...);
    }

    template<class ValueType, class U, class... Args, class T = decay_t<ValueType>>
        requires (is_constructible_v<T, std::initializer_list<U>&, Args...> && is_copy_constructible_v<T>)
    explicit any(in_place_type_t<ValueType> /*unused*/, std::initializer_list<U> il, Args&& ... args) {
        details::handler_type<T>::create(*this, il, std::forward<Args>(args)...);
    }

    auto operator=(const any& rhs) -> any& {
        any(rhs).swap(*this);
        return *this;
    }

    auto operator=(any&& rhs) noexcept -> any& {
        any(std::move(rhs)).swap(*this);
        return *this;
    }

    template<class ValueType, class T = decay_t<ValueType>>
        requires (!is_same_v<T, any> && is_copy_constructible_v<T>)
    auto operator=(ValueType&& rhs) -> any& {
        any(std::forward<ValueType>(rhs)).swap(*this);
        return *this;
    }

    ~any() {
        reset();
    }

    template<class ValueType, class... Args, class T = decay_t<ValueType>>
        requires (is_constructible_v<T, Args...> && is_copy_constructible_v<T>)
    auto emplace(Args&& ... args) -> T& {
        reset();
        return details::handler_type<T>::create(*this, std::forward<Args>(args)...);
    }

    template<class ValueType, class U, class... Args, class T = decay_t<ValueType>>
        requires (is_constructible_v<T, std::initializer_list<U>&, Args...> && is_copy_constructible_v<T>)
    auto emplace(std::initializer_list<U> il, Args&& ... args) -> T& {
        reset();
        return details::handler_type<T>::create(*this, il, std::forward<Args>(args)...);
    }

    auto reset() noexcept -> void {
        if (h_ != nullptr) {
            call(action_type::Destroy);
        }
    }

    auto swap(any& other) noexcept -> void {
        if (this == addressof(other)) {
            return;
        }
        if ((h_ != nullptr) && (other.h_ != nullptr)) {
            any tmp;
            other.call(action_type::Move, &tmp);
            this->call(action_type::Move, &other);
            tmp.call(action_type::Move, this);
        } else if (h_ != nullptr) {
            this->call(action_type::Move, &other);
        } else if (other.h_ != nullptr) {
            other.call(action_type::Move, this);
        }
    }

    [[nodiscard]] auto has_value() const noexcept -> bool {
        return h_ != nullptr;
    }

#ifdef CIEL_HAS_RTTI
    [[nodiscard]] auto type() const noexcept -> const std::type_info& {
        if (h_ != nullptr) {
            return *static_cast<const std::type_info*>(this->call(action_type::TypeInfo));
        }
        return typeid(void);
    }
#endif

};  // class any

template<class T, class U = remove_cvref_t<T>>
    requires is_constructible_v<T, const U&>
[[nodiscard]] auto any_cast(const any& operand) -> T {
    auto res = any_cast<add_const_t<U>>(&operand);
    if (res == nullptr) {
        THROW(std::bad_any_cast());
    }
    return static_cast<T>(*res);
}

template<class T, class U = remove_cvref_t<T>>
    requires is_constructible_v<T, U&>
[[nodiscard]] auto any_cast(any& operand) -> T {
    auto res = any_cast<U>(&operand);
    if (res == nullptr) {
        THROW(std::bad_any_cast());
    }
    return static_cast<T>(*res);
}

template<class T, class U = remove_cvref_t<T>>
    requires is_constructible_v<T, U>
[[nodiscard]] auto any_cast(any&& operand) -> T {
    auto res = any_cast<U>(&operand);
    if (res == nullptr) {
        THROW(std::bad_any_cast());
    }
    return static_cast<T>(std::move(*res));
}

template<class T>
[[nodiscard]] auto any_cast(const any* operand) noexcept -> const T* {
    return any_cast<T>(const_cast<any*>(operand));
}

template<class T>
[[nodiscard]] auto any_cast(any* operand) noexcept -> T* {
    using action_type = details::Action;
    if (operand && operand->h_) {
        void* p = operand->call(action_type::Get, nullptr,
#ifdef CIEL_HAS_RTTI
                                &typeid(T)
#else
                                nullptr
#endif
);
        return static_cast<T*>(p);
    }
    return nullptr;
}

template<class T, class... Args>
[[nodiscard]] auto make_any(Args&& ... args) -> any {
    return any(in_place_type<T>, std::forward<Args>(args)...);
}

template<class T, class U, class... Args>
[[nodiscard]] auto make_any(std::initializer_list<U> il, Args&& ... args) -> any {
    return any(in_place_type<T>, il, std::forward<Args>(args)...);
}

namespace details {

template<class T>
struct small_handler {
    static auto handle(const Action act, const any* _this, any* other, const std::type_info* info) -> void* {
        switch(act) {
            case Action::Destroy:
                destroy(const_cast<any&>(*_this));
                return nullptr;
            case Action::Copy:
                copy(*_this, *other);
                return nullptr;
            case Action::Move:
                move(const_cast<any&>(*_this), *other);
                return nullptr;
            case Action::Get:
                return get(const_cast<any&>(*_this), info);
            case Action::TypeInfo:
#ifdef CIEL_HAS_RTTI
                return type_info();
#else
                [[fallthrough]];
#endif
            default:
                unreachable();
        }
    }

    template<class... Args>
    static auto create(any& dest, Args&&... args) -> T& {
        T* res = static_cast<T*>(static_cast<void*>(&dest.s_.buf_));
        ciel::construct_at(res, std::forward<Args>(args)...);
        dest.h_ = &handle;
        return *res;
    }

private:
    static auto destroy(any& _this) noexcept -> void {
        T* p = static_cast<T*>(static_cast<void*>(&_this.s_.buf_));
        ciel::destroy_at(p);
        _this.h_ = nullptr;
    }

    static auto copy(const any& _this, any& dest) -> void {
        create(dest, *static_cast<const T*>(static_cast<const void*>(&_this.s_.buf_)));
    }

    static auto move(any& _this, any& dest) noexcept -> void {
        create(dest, std::move(*static_cast<T*>(static_cast<void*>(&_this.s_.buf_))));
        destroy(_this);
    }

    [[nodiscard]] static auto get(any& _this, [[maybe_unused]] const std::type_info* info) noexcept -> void* {
        if (true
#ifdef CIEL_HAS_RTTI
        && info != nullptr && *info == typeid(T)
#endif
        ) {
            return static_cast<void*>(&_this.s_.buf_);
        }
        return nullptr;
    }

#ifdef CIEL_HAS_RTTI
    [[nodiscard]] static auto type_info() noexcept -> void* {
        return const_cast<void*>(static_cast<const void*>(&typeid(T)));
    }
#endif

};  // struct small_handler

template<class T>
struct large_handler {
    static auto handle(const Action act, const any* _this, any* other, const std::type_info* info) -> void* {
        switch(act) {
            case Action::Destroy:
                destroy(const_cast<any&>(*_this));
                return nullptr;
            case Action::Copy:
                copy(*_this, *other);
                return nullptr;
            case Action::Move:
                move(const_cast<any&>(*_this), *other);
                return nullptr;
            case Action::Get:
                return get(*_this, info);
            case Action::TypeInfo:
#ifdef CIEL_HAS_RTTI
                return type_info();
#else
                [[fallthrough]];
#endif
            default:
                unreachable();
        }
    }

    template<class... Args>
    static auto create(any& dest, Args&&... args) -> T& {
        allocator<T> a;
        using alloc_traits = allocator_traits<allocator<T>>;
        T* res = alloc_traits::allocate(a, 1);
        CIEL_TRY {
            alloc_traits::construct(a, res, std::forward<Args>(args)...);
        } CIEL_CATCH(...) {
            alloc_traits::deallocate(a, res, 1);
            CIEL_THROW;
        }
        dest.s_.ptr_ = res;
        dest.h_ = &handle;
        return *res;
    }

private:
    static auto destroy(any& _this) noexcept -> void {
        allocator<T> a;
        using alloc_traits = allocator_traits<allocator<T>>;
        T* p = static_cast<T*>(_this.s_.ptr_);
        alloc_traits::destroy(a, p);
        alloc_traits::deallocate(a, p, 1);
        _this.h_ = nullptr;
    }

    static auto copy(const any& _this, any& dest) -> void {
        create(dest, *static_cast<const T*>(_this.s_.ptr_));
    }

    static auto move(any& _this, any& dest) noexcept -> void {
        dest.s_.ptr_ = _this.s_.ptr_;
        dest.h_ = &handle;
        _this.h_ = nullptr;
    }

    [[nodiscard]] static auto get(const any& _this, [[maybe_unused]] const std::type_info* info) noexcept -> void* {
        if (true
#ifdef CIEL_HAS_RTTI
            && info != nullptr && *info == typeid(T)
#endif
        ) {
            return static_cast<void*>(_this.s_.ptr_);
        }
        return nullptr;
    }

#ifdef CIEL_HAS_RTTI
    [[nodiscard]] static auto type_info() noexcept -> void* {
        return const_cast<void*>(static_cast<const void*>(&typeid(T)));
    }
#endif

};  // struct large_handler

}   // namespace details

NAMESPACE_CIEL_END

namespace std {

inline auto swap(ciel::any& lhs, ciel::any& rhs) noexcept -> void {
    lhs.swap(rhs);
}

}   // namespace std

#endif // CIELUTILS_INCLUDE_CIEL_ANY_HPP_