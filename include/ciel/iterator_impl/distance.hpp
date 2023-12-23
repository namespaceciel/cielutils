#ifndef CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_DISTANCE_HPP_
#define CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_DISTANCE_HPP_

#include <ciel/config.hpp>
#include <ciel/iterator_impl/iterator_traits.hpp>
#include <ciel/type_traits_impl/is_base_of.hpp>

NAMESPACE_CIEL_BEGIN

template<class It>
[[nodiscard]] constexpr auto distance(It first, It last) -> typename iterator_traits<It>::difference_type {
    using category = typename iterator_traits<It>::iterator_category;
    static_assert(is_base_of_v<input_iterator_tag, category>, "Iter is not input_iterator");

    if constexpr (is_base_of_v<random_access_iterator_tag, category>) {
        return last - first;
    } else {
        typename iterator_traits<It>::difference_type result = 0;
        while (first != last) {
            ++first;
            ++result;
        }
        return result;
    }
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_DISTANCE_HPP_