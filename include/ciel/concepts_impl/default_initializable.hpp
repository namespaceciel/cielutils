#ifndef CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_DEFAULT_INITIALIZABLE_HPP_
#define CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_DEFAULT_INITIALIZABLE_HPP_

#include <ciel/concepts_impl/constructible_from.hpp>
#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
concept default_initializable = constructible_from<T>
    && requires { T{}; }
    && requires { ::new(static_cast<void*>(nullptr)) T; };

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_DEFAULT_INITIALIZABLE_HPP_