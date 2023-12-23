#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_SIFT_DOWN_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_SIFT_DOWN_HPP_

#include <ciel/config.hpp>
#include <ciel/iterator_impl/iterator_traits.hpp>

NAMESPACE_CIEL_BEGIN

// head is the father to be sifted down
template<class RandomIt, class Compare>
constexpr auto sift_down(RandomIt first, Compare comp,
                         typename iterator_traits<RandomIt>::difference_type len, RandomIt head) -> void {
    using value_type = typename iterator_traits<RandomIt>::value_type;
    using difference_type = typename iterator_traits<RandomIt>::difference_type;
    // If father's index is i, both children's indexs are 2i + 1, 2i + 2
    difference_type diff = head - first;
    if (len < 2 || (len - 2) / 2 < diff) {
        return;
    }
    diff = diff * 2 + 1;    // left child
    RandomIt left_child = first + diff;

    // right exists and is greater than left
    if (diff + 1 < len && comp(*left_child, *(left_child + difference_type(1)))) {
        ++diff;
        ++left_child;    // turn to right child
    }

    if (comp(*left_child, *head)) {    // If father is larger than the biggest child
        return;
    }

    value_type tmp_less = std::move(*head);
    do {
        *head = std::move(*left_child);
        head = left_child;

        if ((len - 2) / 2 < diff) {    // leaf node now
            break;
        }

        diff = diff * 2 + 1;
        left_child = first + diff;

        if (diff + 1 < len && comp(*left_child, *(left_child + difference_type(1)))) {
            ++diff;
            ++left_child;
        }
    } while (!comp(*left_child, tmp_less));    // Loop breaks when smallest element is larger than current child

    *head = std::move(tmp_less);
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_SIFT_DOWN_HPP_