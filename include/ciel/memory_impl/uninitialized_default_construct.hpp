#ifndef CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_UNINITIALIZED_DEFAULT_CONSTRUCT_HPP_
#define CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_UNINITIALIZED_DEFAULT_CONSTRUCT_HPP_

#include <ciel/config.hpp>
#include <ciel/iterator_impl/iterator_traits.hpp>
#include <ciel/memory_impl/addressof.hpp>
#include <ciel/memory_impl/destroy.hpp>

NAMESPACE_CIEL_BEGIN

template<class ForwardIt>
auto uninitialized_default_construct(ForwardIt first, ForwardIt last) -> void {
    using Value = typename iterator_traits<ForwardIt>::value_type;
    ForwardIt current = first;
    CIEL_TRY {
        for (; current != last; ++current) {
            ::new(const_cast<void *>(static_cast<const volatile void *>(ciel::addressof(*current)))) Value;
        }
    } CIEL_CATCH (...) {
        ciel::destroy(first, current);
        CIEL_THROW;
    }
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_UNINITIALIZED_DEFAULT_CONSTRUCT_HPP_