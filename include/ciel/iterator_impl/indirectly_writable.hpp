#ifndef CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_INDIRECTLY_WRITABLE_HPP_
#define CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_INDIRECTLY_WRITABLE_HPP_

#include <ciel/config.hpp>
#include <ciel/iterator_impl/referenceable.hpp>

NAMESPACE_CIEL_BEGIN

template<class Out, class T>
concept indirectly_writable = requires(Out&& o, T&& t) {
    *o = std::forward<T>(t);
    *std::forward<Out>(o) = std::forward<T>(t);
    const_cast<const iter_reference_t<Out>&&>(*o) = std::forward<T>(t);
    const_cast<const iter_reference_t<Out>&&>(*std::forward<Out>(o)) = std::forward<T>(t);
};

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_INDIRECTLY_WRITABLE_HPP_