#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_COPY_CV_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_COPY_CV_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/add_cv.hpp>

NAMESPACE_CIEL_BEGIN

template<class From, class To>
struct copy_cv {
    using type = To;
};

template<class From, class To>
struct copy_cv<const From, To> {
    using type = add_const_t<To>;
};

template<class From, class To>
struct copy_cv<volatile From, To> {
    using type = add_volatile_t<To>;
};

template<class From, class To>
struct copy_cv<const volatile From, To> {
    using type = add_cv_t<To>;
};

template<class From, class To>
using copy_cv_t = typename copy_cv<From, To>::type;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_COPY_CV_HPP_