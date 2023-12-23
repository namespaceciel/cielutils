#ifndef CIELUTILS_INCLUDE_CIEL_UTILITY_IMPL_AS_CONST_HPP_
#define CIELUTILS_INCLUDE_CIEL_UTILITY_IMPL_AS_CONST_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/add_cv.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
constexpr auto as_const(T& t) noexcept -> add_const_t<T>& {
    return t;
}

template<class T>
auto as_const(const T&&) -> void = delete;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_UTILITY_IMPL_AS_CONST_HPP_