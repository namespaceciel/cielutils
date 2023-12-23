#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_MINMAX_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_MINMAX_HPP_

#include <ciel/algorithm_impl/minmax_element.hpp>
#include <ciel/config.hpp>
#include <initializer_list>

NAMESPACE_CIEL_BEGIN

template<class T, class Compare>
[[nodiscard]] constexpr auto minmax(const T& a, const T& b, Compare comp) -> pair<const T&,const T&> {
    return comp(b, a) ? ciel::pair<const T&, const T&>(b, a) : ciel::pair<const T&, const T&>(a, b);
}

template<class T>
[[nodiscard]] constexpr auto minmax(const T& a, const T& b) -> pair<const T&,const T&> {
    return ciel::minmax(a, b, less<>());
}

template<class T, class Compare>
[[nodiscard]] constexpr auto minmax(std::initializer_list<T> ilist, Compare comp) -> pair<T,T> {
    auto p = ciel::minmax_element(ilist.begin(), ilist.end(), comp);
    return ciel::pair(*p.first, *p.second);
}

template<class T>
[[nodiscard]] constexpr auto minmax(std::initializer_list<T> ilist) -> pair<T,T> {
    return ciel::minmax(ilist, less<>());
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_MINMAX_HPP_