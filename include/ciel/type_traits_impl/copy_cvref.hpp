#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_COPY_CVREF_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_COPY_CVREF_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/add_lvalue_reference.hpp>
#include <ciel/type_traits_impl/add_rvalue_reference.hpp>
#include <ciel/type_traits_impl/copy_cv.hpp>

NAMESPACE_CIEL_BEGIN

template<class From, class To>
struct copy_cvref {
    using type = copy_cv_t<From, To>;
};

template<class From, class To>
struct copy_cvref<From&, To> {
    using type = add_lvalue_reference_t<copy_cv_t<From, To>>;
};

template<class From, class To>
struct copy_cvref<From&&, To> {
    using type = add_rvalue_reference_t<copy_cv_t<From, To>>;
};

template<class From, class To>
using copy_cvref_t = typename copy_cvref<From, To>::type;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_COPY_CVREF_HPP_