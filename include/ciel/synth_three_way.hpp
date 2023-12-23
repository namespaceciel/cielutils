#ifndef CIELUTILS_INCLUDE_CIEL_COMPARE_IMPL_SYNTH_THREE_WAY_H
#define CIELUTILS_INCLUDE_CIEL_COMPARE_IMPL_SYNTH_THREE_WAY_H

#include <ciel/concepts_impl/boolean_testable.hpp>
#include <ciel/config.hpp>
#include <ciel/utility_impl/declval.hpp>

NAMESPACE_CIEL_BEGIN

constexpr auto synth_three_way = []<class T, class U>(const T& t, const U& u) requires requires {
    { t < u } -> boolean_testable;
    { u < t } -> boolean_testable;
} {
    if constexpr (std::three_way_comparable_with<T, U>) {
        return t <=> u;
    } else {
        if (t < u) {
            return std::weak_ordering::less;
        }
        if (u < t) {
            return std::weak_ordering::greater;
        }
        return std::weak_ordering::equivalent;
    }
};

template<class T, class U = T>
using synth_three_way_result = decltype(synth_three_way(declval<T&>(), declval<U&>()));

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_COMPARE_IMPL_SYNTH_THREE_WAY_H