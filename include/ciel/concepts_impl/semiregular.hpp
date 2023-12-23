#ifndef CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_SEMIREGULAR_HPP_
#define CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_SEMIREGULAR_HPP_

#include <ciel/concepts_impl/copyable.hpp>
#include <ciel/concepts_impl/default_initializable.hpp>
#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
concept semiregular = copyable<T> && default_initializable<T>;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_SEMIREGULAR_HPP_