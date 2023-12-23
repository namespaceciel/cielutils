#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_ADD_REFERENCE_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_ADD_REFERENCE_HPP_

#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

namespace details {

template<class T>
struct add_reference_helper {
    using type = T&;
};

template<class T>
struct add_reference_helper<T&> {
    using type = T&;
};

template<>
struct add_reference_helper<void> {
    using type = void;
};

}   // namespace details

template<class T>
struct add_reference {
    using type = typename details::add_reference_helper<T>::type;
};

template<class T>
using add_reference_t = typename add_reference<T>::type;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_ADD_REFERENCE_HPP_