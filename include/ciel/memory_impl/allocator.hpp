#ifndef CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_ALLOCATOR_HPP_
#define CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_ALLOCATOR_HPP_

#include <ciel/config.hpp>
#include <ciel/limits.hpp>
#include <ciel/memory_impl/allocation_result.hpp>
#include <ciel/type_traits_impl/is_const.hpp>
#include <ciel/type_traits_impl/is_constant_evaluated.hpp>
#include <ciel/type_traits_impl/is_volatile.hpp>
#include <cstddef>
#include <new>

NAMESPACE_CIEL_BEGIN

template<class T>
class allocator {

    static_assert(!is_const_v<T> && !is_volatile_v<T>, "T is not supposed to be cv-qualified");

public:
    using value_type                             = T;
    using size_type                              = size_t;
    using difference_type                        = ptrdiff_t;
    using propagate_on_container_move_assignment = true_type;

    constexpr allocator() noexcept = default;
    constexpr allocator(const allocator&) noexcept = default;
    constexpr allocator(allocator&&) noexcept = default;

    template<class U>
    constexpr allocator(const allocator<U>& /*unused*/) noexcept {}

    constexpr auto operator=(const allocator&) noexcept -> allocator& = default;
    constexpr auto operator=(allocator&&) noexcept -> allocator& = default;

    constexpr ~allocator() = default;

    [[nodiscard]] constexpr auto allocate(const size_t n) -> T* {
        CIEL_PRECONDITION(n > 0);

        if (numeric_limits<size_t>::max() / sizeof(T) < n) {
            THROW(std::bad_array_new_length());
        }
        if (!is_constant_evaluated() && alignof(T) > __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
            return static_cast<T*>(::operator new(sizeof(T) * n, static_cast<std::align_val_t>(alignof(T))));
        }
        return static_cast<T*>(::operator new(sizeof(T) * n));
    }

    [[nodiscard]] constexpr auto allocate_at_least(const size_t n) -> allocation_result<T*, size_t> {
        CIEL_PRECONDITION(n > 0);

        return {allocate(n), n};
    }

    constexpr auto deallocate(T* p, size_t /*unused*/) noexcept -> void {
        if (!is_constant_evaluated() && alignof(T) > __STDCPP_DEFAULT_NEW_ALIGNMENT__) {
            ::operator delete(static_cast<void*>(p), static_cast<std::align_val_t>(alignof(T)));
        } else {
            ::operator delete(static_cast<void*>(p));
        }
    }

};  // class allocator

template<class T1, class T2>
[[nodiscard]] constexpr auto operator==(const allocator<T1>& /*unused*/,
                                        const allocator<T2>& /*unused*/) noexcept -> bool {
    return true;
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_ALLOCATOR_HPP_