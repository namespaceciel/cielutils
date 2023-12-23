#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_CONDITIONAL_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_CONDITIONAL_HPP_

#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

template<bool B, class T, class F>
struct conditional {
    using type = T;
};

template<class T, class F>
struct conditional<false, T, F> {
    using type = F;
};

template<bool B, class T, class F>
using conditional_t = typename conditional<B, T, F>::type;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_CONDITIONAL_HPP_