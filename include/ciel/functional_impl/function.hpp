#ifndef CIELUTILS_INCLUDE_CIEL_FUNCTIONAL_IMPL_FUNCTION_HPP_
#define CIELUTILS_INCLUDE_CIEL_FUNCTIONAL_IMPL_FUNCTION_HPP_

#include <ciel/config.hpp>
#include <ciel/functional_impl/invoke.hpp>
#include <ciel/functional_impl/strip_signature.hpp>
#include <ciel/memory_impl/allocator.hpp>
#include <ciel/memory_impl/allocator_traits.hpp>
#include <ciel/type_traits_impl/aligned_storage.hpp>
#include <cstddef>
#include <functional>   // std::bad_function_call
#include <typeinfo>

NAMESPACE_CIEL_BEGIN

template<class>
class function;

namespace details {

template<class>
class func_base;

template<class R, class... Args>
class func_base<R(Args...)> {
public:
    func_base() noexcept = default;
    func_base(const func_base&) noexcept = default;
    func_base(func_base&&) noexcept = default;
    auto operator=(const func_base&) noexcept -> func_base& = default;
    auto operator=(func_base&&) noexcept -> func_base& = default;

    virtual ~func_base() = default;
    [[nodiscard]] virtual auto clone() const -> func_base* = 0;
    virtual auto clone_to(func_base*) const -> void = 0;
    virtual auto destroy() noexcept -> void = 0;
    virtual auto destroy_and_deallocate() noexcept -> void = 0;
    virtual auto operator()(Args&&...) -> R = 0;
    [[nodiscard]] virtual auto target(const std::type_info&) const noexcept -> const void* = 0;
#ifdef CIEL_HAS_RTTI
    [[nodiscard]] virtual auto target_type() const noexcept -> const std::type_info& = 0;
#endif

};  // class func_base<R(Args...)>

template<class, class>
class func;

template<class F, class R, class... Args>
class func<F, R(Args...)> final : public func_base<R(Args...)> {
private:
    F f_;

public:
    explicit func(const F& f) : f_(f) {}

    explicit func(F&& f) noexcept : f_(std::move(f)) {}

    [[nodiscard]] virtual auto clone() const -> func_base<R(Args...)>* override {
        allocator<func> a;
        using alloc_traits = allocator_traits<allocator<func>>;
        func* res = alloc_traits::allocate(a, 1);
        CIEL_TRY {
            alloc_traits::construct(a, res, f_);
        } CIEL_CATCH (...) {
            alloc_traits::deallocate(a, res, 1);
            CIEL_THROW;
        }
        return res;
    }

    virtual auto clone_to(func_base<R(Args...)>* fb) const -> void override {
        ciel::construct_at(static_cast<func*>(fb), f_);
    }

    virtual auto destroy() noexcept -> void override {
        ciel::destroy_at(this);
    }

    virtual auto destroy_and_deallocate() noexcept -> void override {
        allocator<func> a;
        using alloc_traits = allocator_traits<allocator<func>>;
        alloc_traits::destroy(a, this);
        alloc_traits::deallocate(a, this, 1);
    }

    virtual auto operator()(Args&&... args) -> R override {
        return f_(std::forward<Args>(args)...);
    }

    [[nodiscard]] virtual auto target([[maybe_unused]] const std::type_info& ti) const noexcept -> const void* override {
        if (true
#ifdef CIEL_HAS_RTTI
            && ti == typeid(F)
#endif
        ) {
            return ciel::addressof(f_);
        }
        return nullptr;
    }

#ifdef CIEL_HAS_RTTI
    [[nodiscard]] virtual auto target_type() const noexcept -> const std::type_info& override {
        return typeid(F);
    }
#endif

};  // class func<F, R(Args...)>

}   // namespace details

template<class R, class... Args>
class function<R(Args...)> {
public:
    using result_type = R;

private:
    using base_type = details::func_base<R(Args...)>;

    buffer_type buffer_;
    base_type* f_{nullptr};

    template<class F>
    [[nodiscard]] static auto not_null(const F& /*unused*/) noexcept -> bool { return true; }

    template<class F>
    [[nodiscard]] static auto not_null(F* ptr) noexcept -> bool { return ptr; }

    template<class Ret, class Class>
    [[nodiscard]] static auto not_null(Ret Class::*ptr) noexcept -> bool { return ptr; }

    template<class F>
    [[nodiscard]] static auto not_null(const function<F>& f) noexcept -> bool { return !!f; }

    auto clear() noexcept -> void {
        if (static_cast<void*>(f_) == &buffer_) {
            f_->destroy();
        } else if (f_) {
            f_->destroy_and_deallocate();
        }
        f_ = nullptr;
    }

public:
    function() noexcept = default;

    function(std::nullptr_t) noexcept {}

    function(const function& other) {
        if (static_cast<void*>(other.f_) == &other.buffer_) {
            f_ = static_cast<base_type*>(static_cast<void*>(&buffer_));
            other.f_->clone_to(f_);
        } else if (other.f_) {
            f_ = other.f_->clone();
        }
    }

    function(function&& other) noexcept {
        if (static_cast<void*>(other.f_) == &other.buffer_) {
            f_ = static_cast<base_type*>(static_cast<void*>(&buffer_));
            other.f_->clone_to(f_);
        } else if (other.f_) {
            f_ = other.f_;
            other.f_ = nullptr;
        }
    }

    template<class F>
        requires (!is_same_v<decay_t<F>, function>) && requires { ciel::INVOKE<R>(declval<F>(), declval<Args>()...); }
    function(F f) {
        using func_type = details::func<F, R(Args...)>;
        if (not_null(f)) {
            if constexpr (is_small_object<F>::value) {
                f_ = ciel::construct_at(static_cast<func_type*>(static_cast<void*>(&buffer_)), std::move(f));
            } else {
                allocator<func_type> a;
                using alloc_traits = allocator_traits<allocator<func_type>>;
                func_type* res = alloc_traits::allocate(a, 1);
                CIEL_TRY {
                    alloc_traits::construct(a, res, std::move(f));
                } CIEL_CATCH (...) {
                    alloc_traits::deallocate(a, res, 1);
                    CIEL_THROW;
                }
                f_ = res;
            }
        }
    }

    ~function() {
        clear();
    }

    auto operator=(const function& other) -> function& {
        if (this == addressof(other)) {
            return *this;
        }
        clear();
        if (static_cast<void*>(other.f_) == &other.buffer_) {
            f_ = static_cast<base_type*>(static_cast<void*>(&buffer_));
            other.f_->clone_to(f_);
        } else if (other.f_) {
            f_ = other.f_->clone();
        }
        return *this;
    }

    auto operator=(function&& other) noexcept -> function& {
        if (this == addressof(other)) {
            return *this;
        }
        clear();
        if (static_cast<void*>(other.f_) == &other.buffer_) {
            f_ = static_cast<base_type*>(static_cast<void*>(&buffer_));
            other.f_->clone_to(f_);
        } else if (other.f_) {
            f_ = other.f_;
            other.f_ = nullptr;
        }
        return *this;
    }

    auto operator=(std::nullptr_t) noexcept -> function& {
        clear();
        return *this;
    }

    template<class F>
        requires requires { ciel::INVOKE<R>(declval<F>(), declval<Args>()...); }
    auto operator=(F&& f) -> function& {
        function(std::forward<F>(f)).swap(*this);
        return *this;
    }

    template<class F>
    auto operator=(reference_wrapper<F> f) noexcept -> function& {
        function(f).swap(*this);
        return *this;
    }

    auto swap(function& other) noexcept -> void {
        if (this == addressof(other)) {
            return;
        }
        if (static_cast<void*>(f_) == &buffer_ && static_cast<void*>(other.f_) == &other.buffer_) {
            buffer_type temp_buffer;
            auto* tmp = static_cast<base_type*>(static_cast<void*>(&temp_buffer));

            f_->clone_to(tmp);
            f_->destroy();

            other.f_->clone_to(f_);
            other.f_->destroy();

            tmp->clone_to(other.f_);
            tmp->destroy();

        } else if (static_cast<void*>(f_) == &buffer_) {
            f_->clone_to(static_cast<base_type*>(static_cast<void*>(&other.buffer_)));
            f_->destroy();
            f_ = other.f_;
            other.f_ = static_cast<base_type*>(static_cast<void*>(&other.buffer_));

        } else if (static_cast<void*>(other.f_) == &other.buffer_) {
            other.f_->clone_to(static_cast<base_type*>(static_cast<void*>(&buffer_)));
            other.f_->destroy();
            other.f_ = f_;
            f_ = static_cast<base_type*>(static_cast<void*>(&buffer_));

        } else {
            std::swap(f_, other.f_);
        }
    }

    [[nodiscard]] explicit operator bool() const noexcept {
        return f_ != nullptr;
    }

    auto operator()(Args... args) const -> R {
        if (!f_) {
            THROW(std::bad_function_call());
        }
        return (*f_)(std::forward<Args>(args)...);
    }

#ifdef CIEL_HAS_RTTI
    [[nodiscard]] auto target_type() const noexcept -> const std::type_info& {
        if (f_ == nullptr) {
            return typeid(void);
        }
        return f_->target_type();
    }
#endif

    template<class T>
    [[nodiscard]] auto target() noexcept -> T* {
#ifdef CIEL_HAS_RTTI
        if (f_ == nullptr) {
            return nullptr;
        }
        return const_cast<T*>(static_cast<const T*>(f_->target(typeid(T))));
#else
        return nullptr;
#endif
    }

    template<class T>
    [[nodiscard]] auto target() const noexcept -> const T* {
#ifdef CIEL_HAS_RTTI
        if (f_ == nullptr) {
            return nullptr;
        }
        return static_cast<const T*>(f_->target(typeid(T)));
#else
        return nullptr;
#endif
    }

};  // class function<R(Args...)>

template<class R, class... ArgTypes>
[[nodiscard]] auto operator==(const function<R(ArgTypes...)>& f, std::nullptr_t) noexcept -> bool {
    return !f;
}

template<class R, class... Args>
function(R(*)(Args...)) -> function<R(Args...)>;

template<class F>
function(F) -> function<strip_signature_t<decltype(&F::operator())>>;

NAMESPACE_CIEL_END

namespace std {

template<class R, class... Args>
auto swap(ciel::function<R(Args...)>& lhs, ciel::function<R(Args...)>& rhs) noexcept -> void {
    lhs.swap(rhs);
}

}   // namespace std

#endif // CIELUTILS_INCLUDE_CIEL_FUNCTIONAL_IMPL_FUNCTION_HPP_