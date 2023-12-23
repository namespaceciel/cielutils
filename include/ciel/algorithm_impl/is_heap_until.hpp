#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_IS_HEAP_UNTIL_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_IS_HEAP_UNTIL_HPP_

#include <ciel/config.hpp>
#include <ciel/functional_impl/less.hpp>

NAMESPACE_CIEL_BEGIN

namespace details {

template<class RandomIt, class Compare>
[[nodiscard]] constexpr auto is_heap_until_helper(RandomIt first, RandomIt last, Compare&& comp) -> RandomIt {
    RandomIt res = first;
    while (++res < last) {
        if (comp(*(first + (res - first - 1) / 2), *res)) {
            return res;
        }
    }
    return res;
}

}   // namespace details

template<class RandomIt, class Compare>
[[nodiscard]] constexpr auto is_heap_until(RandomIt first, RandomIt last, Compare comp) -> RandomIt {
    return details::is_heap_until_helper(first, last, comp);
}

template<class RandomIt>
[[nodiscard]] constexpr auto is_heap_until(RandomIt first, RandomIt last) -> RandomIt {
    return details::is_heap_until_helper(first, last, less<>());
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_IS_HEAP_UNTIL_HPP_