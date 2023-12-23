#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_MAKE_UNSIGNED_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_MAKE_UNSIGNED_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/apply_cv.hpp>
#include <ciel/type_traits_impl/is_enum.hpp>
#include <ciel/type_traits_impl/is_integral.hpp>
#include <ciel/type_traits_impl/remove_cv.hpp>
#include <ciel/type_traits_impl/type_list.hpp>

NAMESPACE_CIEL_BEGIN

namespace details {

template<class T, bool = is_integral_v<T> || is_enum_v<T>>
struct make_unsigned_helper {};

#define PLACEHOLDER unsigned long long
using unsigned_types = type_list<unsigned char,
                       type_list<unsigned short,
                       type_list<unsigned int,
                       type_list<unsigned long,
                       type_list<unsigned long long, PLACEHOLDER >>>>>;
#undef PLACEHOLDER

template<class T>
struct make_unsigned_helper<T, true> {
    using type = typename find_first<unsigned_types, sizeof(T)>::type;
};

template<>
struct make_unsigned_helper<bool, true> {};

template<>
struct make_unsigned_helper<signed short, true> {
    using type = unsigned short;
};

template<>
struct make_unsigned_helper<unsigned short, true> {
    using type = unsigned short;
};

template<>
struct make_unsigned_helper<signed int, true> {
    using type = unsigned int;
};

template<>
struct make_unsigned_helper<unsigned int, true> {
    using type = unsigned int;
};

template<>
struct make_unsigned_helper<signed long, true> {
    using type = unsigned long;
};

template<>
struct make_unsigned_helper<unsigned long, true> {
    using type = unsigned long;
};

template<>
struct make_unsigned_helper<signed long long, true> {
    using type = unsigned long long;
};

template<>
struct make_unsigned_helper<unsigned long long, true> {
    using type = unsigned long long;
};

}   // namespace details

template<class T>
struct make_unsigned {
    using type = apply_cv_t<T, typename details::make_unsigned_helper<remove_cv_t<T>>::type>;
};

template<class T>
using make_unsigned_t = typename make_unsigned<T>::type;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_MAKE_UNSIGNED_HPP_