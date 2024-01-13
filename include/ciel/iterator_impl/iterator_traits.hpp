#ifndef CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_ITERATOR_TRAITS_HPP_
#define CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_ITERATOR_TRAITS_HPP_

#include <ciel/config.hpp>
#include <ciel/iterator_impl/iterator_tag.hpp>
#include <ciel/iterator_impl/legacy_random_access_iterator.hpp>
#include <ciel/type_traits_impl/void_t.hpp>

NAMESPACE_CIEL_BEGIN

namespace details {

template<class, class = void>
struct has_pointer1 {
    using type = void;
};

template<class Iter>
struct has_pointer1<Iter, void_t<typename Iter::pointer>> {
    using type = typename Iter::pointer;
};

template<class Iter> struct has_pointer2_helper3 { using type = void; };
template<class Iter> struct has_pointer2_helper2 : has_pointer2_helper3<Iter> {};
template<class Iter> struct has_pointer2_helper1 : has_pointer2_helper2<Iter> {};

template<class Iter>
    requires requires { typename Iter::pointer; }
struct has_pointer2_helper1<Iter> {
    using type = typename Iter::pointer;
};

template<class Iter>
    requires requires { typename void_t<decltype(declval<Iter&>().operator->())>; }
struct has_pointer2_helper2<Iter> {
    using type = decltype(declval<Iter&>().operator->());
};

template<class Iter, class = void>
struct has_reference2 {
    using type = iter_reference_t<Iter>;
};

template<class Iter>
struct has_reference2<Iter, void_t<typename Iter::reference>> {
    using type = typename Iter::reference;
};

template<class Iter> struct has_iterator_category2_helper5 { using type = input_iterator_tag; };
template<class Iter> struct has_iterator_category2_helper4 : has_iterator_category2_helper5<Iter> {};
template<class Iter> struct has_iterator_category2_helper3 : has_iterator_category2_helper4<Iter> {};
template<class Iter> struct has_iterator_category2_helper2 : has_iterator_category2_helper3<Iter> {};
template<class Iter> struct has_iterator_category2_helper1 : has_iterator_category2_helper2<Iter> {};

template<class Iter>
    requires requires { typename Iter::iterator_category; }
struct has_iterator_category2_helper1<Iter> {
    using type = typename Iter::iterator_category;
};

template<legacy_random_access_iterator Iter>
struct has_iterator_category2_helper2<Iter> {
    using type = random_access_iterator_tag;
};

template<legacy_bidirectional_iterator Iter>
struct has_iterator_category2_helper3<Iter> {
    using type = bidirectional_iterator_tag;
};

template<legacy_forward_iterator Iter>
struct has_iterator_category2_helper4<Iter> {
    using type = forward_iterator_tag;
};

template<class Iter, class = void>
struct has_difference_type3 {
    using type = void;
};

template<class Iter>
struct has_difference_type3<Iter, void_t<typename incrementable_traits<Iter>::difference_type>> {
    using type = typename incrementable_traits<Iter>::difference_type;
};

template<class Iter>
concept has_other_four_members = requires {
    typename Iter::difference_type;
    typename Iter::value_type;
    typename Iter::reference;
    typename Iter::iterator_category;
};

template<class>		 struct iterator_traits_helper3 {};
template<class Iter> struct iterator_traits_helper2 : iterator_traits_helper3<Iter> {};
template<class Iter> struct iterator_traits_helper1 : iterator_traits_helper2<Iter> {};

template<has_other_four_members Iter>
struct iterator_traits_helper1<Iter> {
    using difference_type   = typename Iter::difference_type;
    using value_type        = typename Iter::value_type;
    using pointer           = typename has_pointer1<Iter>::type;
    using reference         = typename Iter::reference;
    using iterator_category = typename Iter::iterator_category;
};

template<legacy_input_iterator Iter>
struct iterator_traits_helper2<Iter> {
    using difference_type   = typename incrementable_traits<Iter>::difference_type;
    using value_type        = typename indirectly_readable_traits<Iter>::value_type;
    using pointer           = typename has_pointer2_helper1<Iter>::type;
    using reference         = typename has_reference2<Iter>::type;
    using iterator_category = typename has_iterator_category2_helper1<Iter>::type;
};

template<legacy_iterator Iter>
struct iterator_traits_helper3<Iter> {
    using difference_type   = typename has_difference_type3<Iter>::type;
    using value_type        = void;
    using pointer           = void;
    using reference         = void;
    using iterator_category = output_iterator_tag;
};

}	// namespace details

template<class Iter>
struct iterator_traits : details::iterator_traits_helper1<Iter> {
    using primary_template = iterator_traits;	// serves for iter_concept
};

template<class T>
    requires is_object_v<T>
struct iterator_traits<T*> {
    using difference_type   = ptrdiff_t;
    using value_type        = remove_cv_t<T>;
    using pointer           = T*;
    using reference         = T&;
    using iterator_category = random_access_iterator_tag;   // compatible with std::iterator_traits
    using iterator_concept  = contiguous_iterator_tag;
};

// The following serve for containers
template<class Iter, class = void>
struct is_exactly_input_iterator : false_type {};

template<class Iter>
struct is_exactly_input_iterator<Iter, void_t<typename iterator_traits<Iter>::iterator_category>>
    : is_same<typename iterator_traits<Iter>::iterator_category, input_iterator_tag> {};

template<class Iter, class = void>
struct is_forward_iterator : false_type {};

template<class Iter>
struct is_forward_iterator<Iter, void_t<typename iterator_traits<Iter>::iterator_category>>
    : is_convertible<typename iterator_traits<Iter>::iterator_category, forward_iterator_tag> {};

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_ITERATOR_TRAITS_HPP_