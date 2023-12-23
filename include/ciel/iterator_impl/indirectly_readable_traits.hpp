#ifndef CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_INDIRECTLY_READABLE_TRAITS_HPP_
#define CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_INDIRECTLY_READABLE_TRAITS_HPP_

#include <ciel/concepts_impl/same_as.hpp>
#include <ciel/config.hpp>
#include <ciel/iterator_impl/has_member_element_type.hpp>
#include <ciel/iterator_impl/has_member_value_type.hpp>
#include <ciel/type_traits_impl/is_array.hpp>
#include <ciel/type_traits_impl/is_object.hpp>
#include <ciel/type_traits_impl/remove_extent.hpp>

NAMESPACE_CIEL_BEGIN

template<class>
struct indirectly_readable_traits {};

template<class T>
struct indirectly_readable_traits<T*> {};

template<class T>
    requires is_object_v<T>
struct indirectly_readable_traits<T*> {
    using value_type = remove_cv_t<T>;
};

template<class I>
    requires is_array_v<I>
struct indirectly_readable_traits<I> {
    using value_type = remove_cv_t<remove_extent_t<I>>;
};

template<class T>
struct indirectly_readable_traits<const T> : indirectly_readable_traits<T> {};

template<has_member_value_type T>
struct indirectly_readable_traits<T> {};

template<has_member_value_type T>
    requires is_object_v<typename T::value_type>
struct indirectly_readable_traits<T> {
    using value_type = remove_cv_t<typename T::value_type>;
};

template<has_member_element_type T>
struct indirectly_readable_traits<T> {};

template<has_member_element_type T>
    requires is_object_v<typename T::element_type>
struct indirectly_readable_traits<T> {
    using value_type = remove_cv_t<typename T::element_type>;
};

template<has_member_value_type T>
    requires has_member_element_type<T>
struct indirectly_readable_traits<T> {};

template<has_member_value_type T>
    requires has_member_element_type<T> &&
        is_object_v<typename T::value_type> &&
            same_as<remove_cv_t<typename T::element_type>, remove_cv_t<typename T::value_type>>
struct indirectly_readable_traits<T> {
    using value_type = remove_cv_t<typename T::value_type>;
};

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_INDIRECTLY_READABLE_TRAITS_HPP_