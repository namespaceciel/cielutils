#ifndef CIELUTILS_INCLUDE_CIEL_LIMITS_HPP_
#define CIELUTILS_INCLUDE_CIEL_LIMITS_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/is_arithmetic.hpp>
#include <ciel/type_traits_impl/is_signed.hpp>

NAMESPACE_CIEL_BEGIN

namespace details {

template<class T, bool = is_arithmetic_v<T>>
class numeric_limits_helper {
protected:
    using type = T;

    static constexpr bool is_specialized = false;
    static constexpr bool is_signed      = false;
    static constexpr bool is_integer     = false;
    static constexpr int digits          = 0;

    [[nodiscard]] static constexpr auto min() noexcept -> type {
        return type();
    }
    [[nodiscard]] static constexpr auto max() noexcept -> type {
        return type();
    }
};

template<class T>
class numeric_limits_helper<T, true> {
protected:
    using type = T;

    static constexpr bool is_specialized = true;
    static constexpr bool is_signed      = is_signed_v<type>;
    static constexpr bool is_integer     = true;
    static constexpr int digits          = static_cast<int>(sizeof(type) * __CHAR_BIT__ -
                                           static_cast<size_t>(is_signed));

    [[nodiscard]] static constexpr auto min() noexcept -> type {
        return is_signed ? type(1) << digits : type(0);
    }
    [[nodiscard]] static constexpr auto max() noexcept -> type {
        return is_signed ? type(-1) ^ min() : type(-1);
    }
};

template<>
class numeric_limits_helper<bool, true> {
protected:
    using type = bool;

    static constexpr bool is_specialized = true;
    static constexpr bool is_signed      = false;
    static constexpr bool is_integer     = true;
    static constexpr int digits          = 1;

    [[nodiscard]] static constexpr auto min() noexcept -> type {
        return false;
    }
    [[nodiscard]] static constexpr auto max() noexcept -> type {
        return true;
    }
};

template<>
class numeric_limits_helper<float, true> {
protected:
    using type = float;

    static constexpr bool is_specialized = true;
    static constexpr bool is_signed      = true;
    static constexpr bool is_integer     = false;
    static constexpr int digits          = __FLT_MANT_DIG__;

    [[nodiscard]] static constexpr auto min() noexcept -> type {
        return __FLT_MIN__;
    }
    [[nodiscard]] static constexpr auto max() noexcept -> type {
        return __FLT_MAX__;
    }
};

template<>
class numeric_limits_helper<double, true> {
protected:
    using type = double;

    static constexpr bool is_specialized = true;
    static constexpr bool is_signed      = true;
    static constexpr bool is_integer     = false;
    static constexpr int digits          = __DBL_MANT_DIG__;

    [[nodiscard]] static constexpr auto min() noexcept -> type {
        return __DBL_MIN__;
    }
    [[nodiscard]] static constexpr auto max() noexcept -> type {
        return __DBL_MAX__;
    }
};

template<>
class numeric_limits_helper<long double, true> {
protected:
    using type = long double;

    static constexpr bool is_specialized = true;
    static constexpr bool is_signed      = true;
    static constexpr bool is_integer     = false;
    static constexpr int digits          = __LDBL_MANT_DIG__;

    [[nodiscard]] static constexpr auto min() noexcept -> type {
        return __LDBL_MIN__;
    }
    [[nodiscard]] static constexpr auto max() noexcept -> type {
        return __LDBL_MAX__;
    }
};

}   // namespace details

template<class T>
class numeric_limits : details::numeric_limits_helper<remove_cv_t<T>> {
private:
    using base = details::numeric_limits_helper<remove_cv_t<T>>;
    using type = typename base::type;

public:
    static constexpr bool is_specialized = base::is_specialized;
    static constexpr bool is_signed      = base::is_signed;
    static constexpr bool is_integer     = base::is_integer;
    static constexpr int digits          = base::digits;

    [[nodiscard]] static constexpr auto min() noexcept -> type {
        return base::min();
    }
    [[nodiscard]] static constexpr auto max() noexcept -> type {
        return base::max();
    }

};  // class numeric_limits

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_LIMITS_HPP_