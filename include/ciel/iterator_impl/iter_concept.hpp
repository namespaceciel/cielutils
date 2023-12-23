#ifndef CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_ITER_CONCEPT_HPP_
#define CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_ITER_CONCEPT_HPP_

#include <ciel/config.hpp>
#include <ciel/iterator_impl/iterator_traits.hpp>

NAMESPACE_CIEL_BEGIN

namespace details {

template<class I>
struct iter_traits : iterator_traits<I> {};

// We define primary_template in main template of iterator_traits to check if it's specialized
template<class I>
    requires requires { typename iterator_traits<I>::primary_template; }
struct iter_traits<I> : I {};

template<class>
struct iter_concept_helper3 {};
template<class I>
struct iter_concept_helper2 : iter_concept_helper3<I> {};
template<class I>
struct iter_concept_helper1 : iter_concept_helper2<I> {};

template<class I>
    requires requires { typename iter_traits<I>::iterator_concept; }
struct iter_concept_helper1<I> {
    using type = typename iter_traits<I>::iterator_concept;
};

template<class I>
    requires requires { typename iter_traits<I>::iterator_category; }
struct iter_concept_helper2<I> {
    using type = typename iter_traits<I>::iterator_category;
};

template<class I>
    requires requires { typename iterator_traits<I>::primary_template; }
struct iter_concept_helper3<I> {
    using type = random_access_iterator_tag;
};

}   // namespace details

template<class I>
using iter_concept = typename details::iter_concept_helper1<I>::type;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_ITER_CONCEPT_HPP_