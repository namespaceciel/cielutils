#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_MAKE_SIGNED_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_MAKE_SIGNED_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/apply_cv.hpp>
#include <ciel/type_traits_impl/is_enum.hpp>
#include <ciel/type_traits_impl/is_integral.hpp>
#include <ciel/type_traits_impl/remove_cv.hpp>
#include <ciel/type_traits_impl/type_list.hpp>

NAMESPACE_CIEL_BEGIN

namespace details {

template<class T, bool = is_integral_v<T> || is_enum_v<T>>
struct make_signed_helper {};

#define PLACEHOLDER signed long long
using signed_types = type_list<signed char,
                     type_list<signed short,
                     type_list<signed int,
                     type_list<signed long,
                     type_list<signed long long, PLACEHOLDER >>>>>;
#undef PLACEHOLDER

template<class T>
struct make_signed_helper<T, true> {
    using type = typename find_first<signed_types, sizeof(T)>::type;
};

template<>
struct make_signed_helper<bool, true> {};

template<>
struct make_signed_helper<signed short, true> {
    using type = signed short;
};

template<>
struct make_signed_helper<unsigned short, true> {
    using type = signed short;
};

template<>
struct make_signed_helper<signed int, true> {
    using type = signed int;
};

template<>
struct make_signed_helper<unsigned int, true> {
    using type = signed int;
};

template<>
struct make_signed_helper<signed long, true> {
    using type = signed long;
};

template<>
struct make_signed_helper<unsigned long, true> {
    using type = signed long;
};

template<>
struct make_signed_helper<signed long long, true> {
    using type = signed long long;
};

template<>
struct make_signed_helper<unsigned long long, true> {
    using type = signed long long;
};

}   // namespace details

template<class T>
struct make_signed {
    using type = apply_cv_t<T, typename details::make_signed_helper<remove_cv_t<T>>::type>;
};

template<class T>
using make_signed_t = typename make_signed<T>::type;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_MAKE_SIGNED_HPP_