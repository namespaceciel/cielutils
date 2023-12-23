#ifndef CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_ADVANCE_HPP_
#define CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_ADVANCE_HPP_

#include <ciel/config.hpp>
#include <ciel/iterator_impl/iterator_traits.hpp>
#include <ciel/type_traits_impl/is_base_of.hpp>

NAMESPACE_CIEL_BEGIN

template<class Iter, class Distance>
constexpr auto advance(Iter& it, Distance n) -> void {
    using category = typename iterator_traits<Iter>::iterator_category;
    static_assert(is_base_of_v<input_iterator_tag, category>, "Iter is not input_iterator");

    auto dist = typename iterator_traits<Iter>::difference_type(n);
    if constexpr (is_base_of_v<random_access_iterator_tag, category>) {
        it += dist;
    } else {
        while (dist > 0) {
            --dist;
            ++it;
        }
        if constexpr (ciel::is_base_of_v<bidirectional_iterator_tag, category>) {    // n < 0
            while (dist < 0) {
                ++dist;
                --it;
            }
        }
    }
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_ADVANCE_HPP_