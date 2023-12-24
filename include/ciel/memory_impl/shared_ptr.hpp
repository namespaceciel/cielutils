#ifndef CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_SHARED_PTR_HPP_
#define CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_SHARED_PTR_HPP_

// I have no idea why these two signed type are used:
// element_type& operator[](ptrdiff_t idx) const;
// long use_count() const noexcept;

#include <ciel/config.hpp>
#include <ciel/memory_impl/allocator.hpp>
#include <ciel/memory_impl/unique_ptr.hpp>
#include <typeinfo>

NAMESPACE_CIEL_BEGIN

// TODO: non member functions

// In a typical implementation, shared_ptr holds only two pointers:
//      the stored pointer (one returned by get());
//      a pointer to control block.

// The control block is a dynamically-allocated object that holds:
//      either a pointer to the managed object or the managed object itself;
//      the deleter (type-erased);
//      the allocator (type-erased);
//      the number of shared_ptrs that own the managed object;
//      the number of weak_ptrs that refer to the managed object.
//
// The stored pointer is different from the managed pointer when:
//      1. the object that the stored pointer points to is the member of the managed pointer
//      2. polymorphism
//
// Because of type-erasure, get_deleter() need users to specify Deleter and check if typeid is matched,
// or it returns nullptr
//
// The destruction and deallocations should be managed by the control block itself,
// since base pointer can't get the size of control block

template<class> class weak_ptr;
template<class> class enable_shared_from_this;

template<class Y, class T>
struct is_compatible_with : bool_constant<is_convertible_v<Y*, T*> ||
    (is_bounded_array_v<Y> && is_unbounded_array_v<T> &&
    is_same_v<remove_extent_t<Y>, remove_cv_t<remove_extent_t<T>>>)> {};

struct shared_weak_count {

    std::atomic<size_t> shared_count_;
    std::atomic<size_t> shared_and_weak_count_;

    explicit shared_weak_count(const size_t s = 1, const size_t w = 1)
        : shared_count_(s), shared_and_weak_count_(w) {}

    shared_weak_count(const shared_weak_count&) noexcept = delete;
    shared_weak_count(shared_weak_count&&) noexcept = delete;
    auto operator=(const shared_weak_count&) noexcept -> shared_weak_count& = delete;
    auto operator=(shared_weak_count&&) noexcept -> shared_weak_count& = delete;

    virtual ~shared_weak_count() noexcept = default;

    [[nodiscard]] auto use_count() const noexcept -> size_t {
        return shared_count_.load(std::memory_order_relaxed);
    }

    auto add_ref() noexcept -> void {
        shared_count_.fetch_add(1, std::memory_order_relaxed);
        shared_and_weak_count_.fetch_add(1, std::memory_order_relaxed);
    }

    auto weak_add_ref() noexcept -> void {
        shared_and_weak_count_.fetch_add(1, std::memory_order_relaxed);
    }

    auto shared_count_release() -> void {
        if (shared_count_.fetch_sub(1, std::memory_order_release) == 1) {
            delete_pointer();
        }
        shared_and_weak_count_release();
    }

    auto shared_and_weak_count_release() -> void {
        if (shared_and_weak_count_.fetch_sub(1, std::memory_order_release) == 1) {
            delete_control_block();
        }
    }

    [[nodiscard]] virtual auto get_deleter(const std::type_info& type) noexcept -> void* = 0;
    virtual auto delete_pointer() noexcept -> void = 0;
    virtual auto delete_control_block() noexcept -> void = 0;

};    // shared_weak_count

template<class element_type, class Deleter, class Allocator>
    requires requires { declval<Deleter>()(static_cast<element_type*>(nullptr)); } &&
        is_move_constructible_v<Deleter>
struct control_block_with_pointer final : shared_weak_count {
public:
    using pointer                    = element_type*;
    using deleter_type               = Deleter;
    using allocator_type             = Allocator;

private:
    using alloc_traits               = allocator_traits<allocator_type>;
    using control_block_allocator    =
        typename alloc_traits::template rebind_alloc<
            control_block_with_pointer<element_type, Deleter, Allocator>>;
    using control_block_alloc_traits =
        typename alloc_traits::template rebind_traits<
            control_block_with_pointer<element_type, Deleter, Allocator>>;

    pointer ptr_;
    [[no_unique_address]] deleter_type dlt_;
    [[no_unique_address]] allocator_type allocator_;

public:
    control_block_with_pointer(pointer p, deleter_type&& d, allocator_type&& alloc)
        : ptr_(p), dlt_(std::move(d)), allocator_(std::move(alloc)) {}

    [[nodiscard]] virtual auto get_deleter([[maybe_unused]] const std::type_info& type) noexcept -> void* override {
#ifdef CIEL_HAS_RTTI
        return (type == typeid(deleter_type)) ? static_cast<void*>(&dlt_) : nullptr;
#else
        return nullptr;
#endif
    }

    virtual auto delete_pointer() noexcept -> void override {
        dlt_(ptr_);
//		ptr_ = nullptr;
    }

    virtual auto delete_control_block() noexcept -> void override {
        control_block_allocator allocator(allocator_);
        this->~control_block_with_pointer();
        control_block_alloc_traits::deallocate(allocator, this, 1);
    }

};    // control_block_with_pointer

// TODO: struct control_block_with_instance

template<class T>
class shared_ptr {
public:
    using element_type = remove_extent_t<T>;
    using weak_type    = weak_ptr<T>;

private:
    template<class> friend class shared_ptr;
    template<class> friend class weak_ptr;

    element_type* ptr_;
    shared_weak_count* count_;

    template<class Deleter, class Allocator>
    [[nodiscard]] auto alloc_control_block(element_type* ptr, Deleter&& dlt, Allocator&& alloc)
        -> shared_weak_count* {
        using alloc_traits = allocator_traits<Allocator>;
        using control_block_allocator    =
            typename alloc_traits::template rebind_alloc<
                control_block_with_pointer<element_type, Deleter, Allocator>>;
        using control_block_alloc_traits =
            typename alloc_traits::template rebind_traits<
                control_block_with_pointer<element_type, Deleter, Allocator>>;

        control_block_allocator allocator(alloc);
        control_block_with_pointer<element_type, Deleter, Allocator>* ctlblk =
            control_block_alloc_traits::allocate(allocator, 1);
        CIEL_TRY {
            control_block_alloc_traits::construct(allocator, ctlblk, ptr, std::move(dlt), std::move(alloc));
            return ctlblk;
        } CIEL_CATCH (...) {
            control_block_alloc_traits::deallocate(allocator, ctlblk, 1);
            CIEL_THROW;
        }
    }

    // serves for enable_shared_from_this
    template<class Now, class Original>
        requires is_convertible_v<Original*, const enable_shared_from_this<Now>*>
    auto enable_weak_this(const enable_shared_from_this<Now>* now_ptr, Original* original_ptr) noexcept -> void {
        using RawNow = remove_cv_t<Now>;
        // If now_ptr is not initialized, let it points to the right control block
        if (now_ptr && now_ptr->weak_this_.expired()) {
            now_ptr->weak_this_ =
                shared_ptr<RawNow>(*this, const_cast<RawNow*>(static_cast<const Now*>(original_ptr)));
        }
    }

    auto enable_weak_this(...) noexcept -> void {}

public:
    constexpr shared_ptr() noexcept: ptr_(nullptr), count_(nullptr) {}

    constexpr shared_ptr(std::nullptr_t) noexcept: ptr_(nullptr), count_(nullptr) {}

    template<class Y>
    explicit shared_ptr(Y* ptr) : ptr_(ptr) {
        unique_ptr<Y> holder(ptr);
        count_ = alloc_control_block(ptr, default_delete<T>(), allocator<T>());
        (void)holder.release();
        enable_weak_this(ptr, ptr);
    }

    template<class Y, class Deleter>
    shared_ptr(Y* ptr, Deleter d) : ptr_(ptr) {
        unique_ptr<Y, Deleter> holder(ptr, d);
        count_ = alloc_control_block(ptr, std::move(d), allocator<T>());
        (void)holder.release();
        enable_weak_this(ptr, ptr);
    }

    template<class Deleter>
    shared_ptr(std::nullptr_t, Deleter /*unused*/) : ptr_(nullptr), count_(nullptr) {}

    template<class Y, class Deleter, class Alloc>
    shared_ptr(Y* ptr, Deleter d, Alloc alloc) : ptr_(ptr) {
        unique_ptr<Y, Deleter> holder(ptr, d);
        count_ = alloc_control_block(ptr, std::move(d), std::move(alloc));
        (void)holder.release();
        enable_weak_this(ptr, ptr);
    }

    template<class Deleter, class Alloc>
    shared_ptr(std::nullptr_t, Deleter /*unused*/, Alloc /*unused*/) : ptr_(nullptr), count_(nullptr) {}

    template<class Y>
    shared_ptr(const shared_ptr<Y>& r, element_type* ptr) noexcept : ptr_(ptr), count_(r.count_) {
        if (count_ != nullptr) {
            count_->add_ref();
        }
    }

    template<class Y>
    shared_ptr(shared_ptr<Y>&& r, element_type* ptr) noexcept : ptr_(ptr), count_(r.count_) {
        r.ptr_ = nullptr;
        r.count_ = nullptr;
    }

    shared_ptr(const shared_ptr& r) noexcept: ptr_(r.ptr_), count_(r.count_) {
        if (count_ != nullptr) {
            count_->add_ref();
        }
    }

    template<class Y>
        requires is_compatible_with<Y, T>::value
    shared_ptr(const shared_ptr<Y>& r) noexcept : ptr_(r.ptr_), count_(r.count_) {
        if (count_ != nullptr) {
            count_->add_ref();
        }
    }

    shared_ptr(shared_ptr&& r) noexcept: ptr_(r.ptr_), count_(r.count_) {
        r.ptr_ = nullptr;
        r.count_ = nullptr;
    }

    template<class Y>
        requires is_compatible_with<Y, T>::value
    shared_ptr(shared_ptr<Y>&& r) noexcept : ptr_(r.ptr_), count_(r.count_) {
        r.ptr_ = nullptr;
        r.count_ = nullptr;
    }

    template<class Y>
        requires is_compatible_with<Y, T>::value
    explicit shared_ptr(const weak_ptr<Y>& r) : ptr_(r.ptr_), count_(r.count_) {
        if (count_ != nullptr) {
            count_->add_ref();
        }
    }

    template<class Y, class Deleter>
        requires is_compatible_with<remove_pointer_t<typename unique_ptr<Y, Deleter>::pointer>, T>::value
    shared_ptr(unique_ptr<Y, Deleter>&& r) : ptr_(r.get()) {
        if (ptr_ != nullptr) {
            count_ = alloc_control_block(ptr_, std::move(r.get_deleter()), allocator<T>());
            enable_weak_this(ptr_, ptr_);
        } else {
            count_ = nullptr;
        }
        r.release();
    }

    ~shared_ptr() {
        if (count_ != nullptr) {
            count_->shared_count_release();
        }
    }

    auto operator=(const shared_ptr& r) noexcept -> shared_ptr& {
        shared_ptr(r).swap(*this);
        return *this;
    }

    template<class Y>
    auto operator=(const shared_ptr<Y>& r) noexcept -> shared_ptr& {
        shared_ptr(r).swap(*this);
        return *this;
    }

    auto operator=(shared_ptr&& r) noexcept -> shared_ptr& {
        shared_ptr(std::move(r)).swap(*this);
        return *this;
    }

    template<class Y>
    auto operator=(shared_ptr<Y>&& r) noexcept -> shared_ptr& {
        shared_ptr(std::move(r)).swap(*this);
        return *this;
    }

    template<class Y, class Deleter>
    auto operator=(unique_ptr<Y, Deleter>&& r) -> shared_ptr& {
        shared_ptr(std::move(r)).swap(*this);
        return *this;
    }

    auto reset() noexcept -> void {
        shared_ptr().swap(*this);
    }

    template<class Y>
        requires is_convertible_v<Y, T>
    auto reset(Y* ptr) -> void {
        shared_ptr(ptr).swap(*this);
    }

    template<class Y, class Deleter>
        requires is_convertible_v<Y, T>
    auto reset(Y* ptr, Deleter d) -> void {
        shared_ptr(ptr, d).swap(*this);
    }

    template<class Y, class Deleter, class Alloc>
        requires is_convertible_v<Y, T>
    auto reset(Y* ptr, Deleter d, Alloc alloc) -> void {
        shared_ptr(ptr, d, alloc).swap(*this);
    }

    auto swap(shared_ptr& r) noexcept -> void {
        using std::swap;
        swap(ptr_, r.ptr_);
        swap(count_, r.count_);
    }

    [[nodiscard]] auto get() const noexcept -> element_type* {
        return ptr_;
    }

    [[nodiscard]] auto operator*() const noexcept -> T& {
        return *get();
    }

    [[nodiscard]] auto operator->() const noexcept -> T* {
        return get();
    }

    [[nodiscard]] auto operator[](const size_t idx) const -> element_type& {
        static_assert(is_array_v<T>, "ciel::shared_ptr::operator[] is valid only when T is array");

        return get()[idx];
    }

    [[nodiscard]] auto use_count() const noexcept -> size_t {
        return count_ != nullptr ? count_->use_count() : 0;
    }

    [[nodiscard]] explicit operator bool() const noexcept {
        return get() != nullptr;
    }

    template<class Y>
    [[nodiscard]] auto owner_before(const shared_ptr<Y>& other) const noexcept -> bool {
        return count_ < other.count_;
    }

    template<class Y>
    [[nodiscard]] auto owner_before(const weak_ptr<Y>& other) const noexcept -> bool {
        return count_ < other.count_;
    }

    template<class D>
    [[nodiscard]] auto get_deleter() const noexcept -> D* {
#ifdef CIEL_HAS_RTTI
        return count_ ? static_cast<D*>(count_->get_deleter(typeid(remove_cv_t<D>))) : nullptr;
#else
        return nullptr;
#endif
    }

};    // class shared_ptr

template<class Deleter, class T>
[[nodiscard]] auto get_deleter(const shared_ptr<T>& p) noexcept -> Deleter* {
    return p.template get_deleter<Deleter>();
}

template<class T>
shared_ptr(weak_ptr<T>) -> shared_ptr<T>;

template<class T, class D>
shared_ptr(unique_ptr<T, D>) -> shared_ptr<T>;

template<class T>
class weak_ptr {
public:
    using element_type = remove_extent_t<T>;

private:
    element_type* ptr_;
    shared_weak_count* count_;

public:
    template<class> friend class shared_ptr;
    template<class> friend class weak_ptr;

    constexpr weak_ptr() noexcept: ptr_(nullptr), count_(nullptr) {}

    weak_ptr(const weak_ptr& r) noexcept: ptr_(r.ptr_), count_(r.count_) {
        if (count_ != nullptr) {
            count_->weak_add_ref();
        }
    }

    template<class Y>
        requires is_compatible_with<Y, T>::value
    weak_ptr(const weak_ptr<Y>& r) noexcept : ptr_(r.ptr_), count_(r.count_) {
        if (count_ != nullptr) {
            count_->weak_add_ref();
        }
    }

    template<class Y>
        requires is_compatible_with<Y, T>::value
    weak_ptr(const shared_ptr<Y>& r) noexcept : ptr_(r.ptr_), count_(r.count_) {
        if (count_ != nullptr) {
            count_->weak_add_ref();
        }
    }

    weak_ptr(weak_ptr&& r) noexcept: ptr_(r.ptr_), count_(r.count_) {
        r.ptr_ = nullptr;
        r.count_ = nullptr;
    }

    template<class Y>
        requires is_convertible_v<Y*, T*>
    weak_ptr(weak_ptr<Y>&& r) noexcept : ptr_(r.ptr_), count_(r.count_) {
        r.ptr_ = nullptr;
        r.count_ = nullptr;
    }

    ~weak_ptr() {
        if (count_ != nullptr) {
            count_->shared_and_weak_count_release();
        }
    }

    auto operator=(const weak_ptr& r) noexcept -> weak_ptr& {
        weak_ptr(r).swap(*this);
        return *this;
    }

    template<class Y>
    auto operator=(const weak_ptr<Y>& r) noexcept -> weak_ptr& {
        weak_ptr(r).swap(*this);
        return *this;
    }

    template<class Y>
    auto operator=(const shared_ptr<Y>& r) noexcept -> weak_ptr& {
        weak_ptr(r).swap(*this);
        return *this;
    }

    auto operator=(weak_ptr&& r) noexcept -> weak_ptr& {
        weak_ptr(std::move(r)).swap(*this);
        return *this;
    }

    template<class Y>
    auto operator=(weak_ptr<Y>&& r) noexcept -> weak_ptr& {
        weak_ptr(std::move(r)).swap(*this);
        return *this;
    }

    auto reset() noexcept -> void {
        if (count_ != nullptr) {
            count_->shared_and_weak_count_release();
            ptr_ = nullptr;
            count_ = nullptr;
        }
    }

    auto swap(weak_ptr& r) noexcept -> void {
        using std::swap;
        swap(ptr_, r.ptr_);
        swap(count_, r.count_);
    }

    [[nodiscard]] auto use_count() const noexcept -> size_t {
        return count_ != nullptr ? count_->use_count() : 0;
    }

    [[nodiscard]] auto expired() const noexcept -> bool {
        return use_count() == 0;
    }

    [[nodiscard]] auto lock() const noexcept -> shared_ptr<T> {
        return expired() ? shared_ptr<T>() : shared_ptr<T>(*this);
    }

    template<class Y>
    [[nodiscard]] auto owner_before(const weak_ptr<Y>& other) const noexcept -> bool {
        return count_ < other.count_;
    }

    template<class Y>
    [[nodiscard]] auto owner_before(const shared_ptr<Y>& other) const noexcept -> bool {
        return count_ < other.count_;
    }

};    // class weak_ptr

template<class T>
weak_ptr(shared_ptr<T>) -> weak_ptr<T>;

template<class T>
class enable_shared_from_this {
private:
    mutable weak_ptr<T> weak_this_;

protected:
    constexpr enable_shared_from_this() noexcept = default;
    enable_shared_from_this(const enable_shared_from_this&) noexcept = default;
    enable_shared_from_this(enable_shared_from_this&&) noexcept = default;
    ~enable_shared_from_this() = default;
    auto operator=(const enable_shared_from_this&) noexcept -> enable_shared_from_this& = default;
    auto operator=(enable_shared_from_this&&) noexcept -> enable_shared_from_this& = default;

public:
    template<class> friend class shared_ptr;

    [[nodiscard]] auto shared_from_this() -> shared_ptr<T> {
        return shared_ptr<T>(weak_this_);
    }

    [[nodiscard]] auto shared_from_this() const -> shared_ptr<const T> {
        return shared_ptr<const T>(weak_this_);
    }

    [[nodiscard]] auto weak_from_this() noexcept -> weak_ptr<T> {
        return weak_this_;
    }

    [[nodiscard]] auto weak_from_this() const noexcept -> weak_ptr<const T> {
        return weak_this_;
    }

};  // class enable_shared_from_this

NAMESPACE_CIEL_END

namespace std {

template<class T>
auto swap(ciel::shared_ptr<T>& lhs, ciel::shared_ptr<T>& rhs) noexcept -> void {
    lhs.swap(rhs);
}

template<class T>
auto swap(ciel::weak_ptr<T>& lhs, ciel::weak_ptr<T>& rhs) noexcept -> void {
    lhs.swap(rhs);
}

}   // namespace std

#endif // CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_SHARED_PTR_HPP_