#ifndef CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_ITER_ALIAS_HPP_
#define CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_ITER_ALIAS_HPP_

#include <ciel/config.hpp>
#include <ciel/iterator_impl/indirectly_readable_traits.hpp>
#include <ciel/iterator_impl/iterator_traits.hpp>
#include <ciel/iterator_impl/ranges_iter_move.hpp>
#include <ciel/iterator_impl/referenceable.hpp>    // iter_reference_t

NAMESPACE_CIEL_BEGIN

namespace details {

template<class T, class = void>
struct iter_value_t_helper {
    using value_type = typename indirectly_readable_traits<remove_cvref_t<T>>::value_type;
};

template<class T>
struct iter_value_t_helper<T, void_t<typename iterator_traits<remove_cvref_t<T>>::value_type>> {
    using value_type = typename iterator_traits<remove_cvref_t<T>>::value_type;
};

template<class T, class = void>
struct iter_difference_t_helper {
    using value_type = typename incrementable_traits<remove_cvref_t<T>>::difference_type;
};

template<class T>
struct iter_difference_t_helper<T, void_t<typename iterator_traits<remove_cvref_t<T>>::difference_type>> {
    using value_type = typename iterator_traits<remove_cvref_t<T>>::difference_type;
};

}   // namespace details

template<class T>
using iter_value_t = typename details::iter_value_t_helper<T>::value_type;

template<class T>
using iter_difference_t = typename details::iter_difference_t_helper<T>::value_type;

template<dereferenceable T>
    requires requires {{ ranges::iter_move(declval<T&>()) } -> can_reference; }
using iter_rvalue_reference_t = decltype(ranges::iter_move(declval<T&>()));

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_ITER_ALIAS_HPP_