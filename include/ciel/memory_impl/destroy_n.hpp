#ifndef CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_DESTROY_N_HPP_
#define CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_DESTROY_N_HPP_

#include <ciel/config.hpp>
#include <ciel/memory_impl/destroy_at.hpp>

NAMESPACE_CIEL_BEGIN

template<class ForwardIt, class Size>
constexpr auto destroy_n(ForwardIt first, Size n) -> ForwardIt {
    for (; n > 0; (void)++first, --n) {
        ciel::destroy_at(ciel::addressof(*first));
    }
    return first;
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_DESTROY_N_HPP_