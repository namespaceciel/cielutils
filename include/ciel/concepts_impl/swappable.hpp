#ifndef CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_SWAPPABLE_HPP_
#define CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_SWAPPABLE_HPP_

#include <ciel/concepts_impl/ranges_swap.hpp>
#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
concept swappable = requires(T& a, T& b) { ranges::swap(a, b); };

template<class T, class U>
concept swappable_with = common_reference_with<T, U>
    && requires(T&& t, U&& u) {
        ranges::swap(std::forward<T>(t), std::forward<T>(t));
        ranges::swap(std::forward<U>(u), std::forward<U>(u));
        ranges::swap(std::forward<T>(t), std::forward<U>(u));
        ranges::swap(std::forward<U>(u), std::forward<T>(t));
    };

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_SWAPPABLE_HPP_