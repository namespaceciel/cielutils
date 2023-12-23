#ifndef CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_TOTALLY_ORDERED_HPP_
#define CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_TOTALLY_ORDERED_HPP_

#include <ciel/concepts_impl/equality_comparable.hpp>
#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

namespace details {

template<class T, class U>
concept partially_ordered_with = requires(const remove_reference_t<T>& t, const remove_reference_t<U>& u) {
    { t < u } -> boolean_testable;
    { t > u } -> boolean_testable;
    { t <= u } -> boolean_testable;
    { t >= u } -> boolean_testable;
    { u < t } -> boolean_testable;
    { u > t } -> boolean_testable;
    { u <= t } -> boolean_testable;
    { u >= t } -> boolean_testable;
};

}   // namespace details

template<class T>
concept totally_ordered = equality_comparable<T> && details::partially_ordered_with<T, T>;

template<class T, class U>
concept totally_ordered_with = totally_ordered<T>
    && totally_ordered<U>
    && equality_comparable_with<T, U>
    && totally_ordered<common_reference_t<const remove_reference_t<T>&, const remove_reference_t<U>&>>
    && details::partially_ordered_with<T, U>;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_TOTALLY_ORDERED_HPP_