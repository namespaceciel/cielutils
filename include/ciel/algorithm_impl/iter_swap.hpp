#ifndef CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_ITER_SWAP_HPP_
#define CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_ITER_SWAP_HPP_

#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

template<class ForwardIt1, class ForwardIt2>
constexpr auto iter_swap(ForwardIt1 a, ForwardIt2 b) -> void {
    using std::swap;
    swap(*a, *b);
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ALGORITHM_IMPL_ITER_SWAP_HPP_