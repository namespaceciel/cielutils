#ifndef CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_DESTROY_HPP_
#define CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_DESTROY_HPP_

#include <ciel/config.hpp>
#include <ciel/memory_impl/destroy_at.hpp>

NAMESPACE_CIEL_BEGIN

template<class ForwardIt>
constexpr auto destroy(ForwardIt first, ForwardIt last) -> void {
    for (; first != last; ++first) {
        ciel::destroy_at(ciel::addressof(*first));
    }
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_DESTROY_HPP_