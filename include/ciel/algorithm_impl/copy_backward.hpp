#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_COPY_BACKWARD_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_COPY_BACKWARD_HPP_

#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

// It's undefined behavior when d_last in range (first, last], use copy instead
template<class BidirIt1, class BidirIt2>
constexpr auto copy_backward(BidirIt1 first, BidirIt1 last, BidirIt2 d_last) -> BidirIt2 {
    while (first != last) {
        *(--d_last) = *(--last);
    }
    return d_last;
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_COPY_BACKWARD_HPP_