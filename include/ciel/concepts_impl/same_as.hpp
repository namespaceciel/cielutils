#ifndef CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_SAME_AS_HPP_
#define CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_SAME_AS_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/is_same.hpp>

NAMESPACE_CIEL_BEGIN

namespace details {

template<class T, class U>
concept same_as_helper = is_same_v<T, U>;

}   // namespace details

template<class T, class U>
concept same_as = details::same_as_helper<T, U> && details::same_as_helper<U, T>;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_SAME_AS_HPP_