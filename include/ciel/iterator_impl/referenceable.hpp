#ifndef CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_REFERENCEABLE_HPP_
#define CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_REFERENCEABLE_HPP_

#include <ciel/config.hpp>
#include <ciel/utility_impl/declval.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
using with_reference = T&;

template<class T>
concept can_reference = requires { typename with_reference<T>; };

template<class T>
concept dereferenceable = requires(T& t) {
    { *t } -> can_reference;
};

template<dereferenceable T>
using iter_reference_t = decltype(*declval<T&>());

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_REFERENCEABLE_HPP_