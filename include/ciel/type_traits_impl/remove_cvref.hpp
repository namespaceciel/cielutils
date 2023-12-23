#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_REMOVE_CVREF_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_REMOVE_CVREF_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/remove_cv.hpp>
#include <ciel/type_traits_impl/remove_reference.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
struct remove_cvref {
    using type = remove_cv_t<remove_reference_t<T>>;
};

template<class T>
using remove_cvref_t = typename remove_cvref<T>::type;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_REMOVE_CVREF_HPP_