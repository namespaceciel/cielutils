#ifndef CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_REGULAR_HPP_
#define CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_REGULAR_HPP_

#include <ciel/concepts_impl/equality_comparable.hpp>
#include <ciel/concepts_impl/semiregular.hpp>
#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
concept regular = semiregular<T> && equality_comparable<T>;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_REGULAR_HPP_