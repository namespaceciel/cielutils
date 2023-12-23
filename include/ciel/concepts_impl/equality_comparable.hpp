#ifndef CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_EQUALITY_COMPARABLE_HPP_
#define CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_EQUALITY_COMPARABLE_HPP_

#include <ciel/concepts_impl/boolean_testable.hpp>
#include <ciel/concepts_impl/common_reference_with.hpp>
#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

namespace details {

template<class T, class U>
concept weakly_equality_comparable_with = requires(const remove_reference_t<T>& t, const remove_reference_t<U>& u) {
    { t == u } -> boolean_testable;
    { t != u } -> boolean_testable;
    { u == t } -> boolean_testable;
    { u != t } -> boolean_testable;
};

}   // namespace details

template<class T>
concept equality_comparable = details::weakly_equality_comparable_with<T, T>;

template<class T, class U>
concept equality_comparable_with = equality_comparable<T>
    && equality_comparable<U>
    && common_reference_with<const remove_reference_t<T>&, const remove_reference_t<U>&>
    && equality_comparable<common_reference_t<const remove_reference_t<T>&, const remove_reference_t<U>&>>
    && details::weakly_equality_comparable_with<T, U>;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_EQUALITY_COMPARABLE_HPP_