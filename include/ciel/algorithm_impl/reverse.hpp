#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_REVERSE_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_REVERSE_HPP_

#include <ciel/algorithm_impl/iter_swap.hpp>
#include <ciel/config.hpp>
#include <ciel/iterator_impl/iterator_traits.hpp>
#include <ciel/type_traits_impl/is_base_of.hpp>

NAMESPACE_CIEL_BEGIN

template<class BidirIt>
constexpr auto reverse(BidirIt first, BidirIt last) -> void {
    using iter_cat = typename iterator_traits<BidirIt>::iterator_category;

    if constexpr (is_base_of_v<random_access_iterator_tag, iter_cat>) {
        if (first == last) {
            return;
        }
        for (--last; first < last; ++first, --last) {
            ciel::iter_swap(first, last);
        }
    } else {
        while ((first != last) && (first != --last)) {
            ciel::iter_swap(first++, last);
        }
    }
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_REVERSE_HPP_