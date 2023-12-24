#ifndef CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_UNINITIALIZED_DEFAULT_CONSTRUCT_N_HPP_
#define CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_UNINITIALIZED_DEFAULT_CONSTRUCT_N_HPP_

#include <ciel/config.hpp>
#include <ciel/iterator_impl/iterator_traits.hpp>
#include <ciel/memory_impl/addressof.hpp>
#include <ciel/memory_impl/destroy.hpp>

NAMESPACE_CIEL_BEGIN

template<class ForwardIt, class Size>
auto uninitialized_default_construct_n(ForwardIt first, Size n) -> ForwardIt {
    using T = typename iterator_traits<ForwardIt>::value_type;
    ForwardIt current = first;
    CIEL_TRY {
        for (; n > 0; (void) ++current, --n) {
            ::new(const_cast<void *>(static_cast<const volatile void *>(ciel::addressof(*current)))) T;
        }
        return current;
    } CIEL_CATCH (...) {
        ciel::destroy(first, current);
        CIEL_THROW;
    }
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_UNINITIALIZED_DEFAULT_CONSTRUCT_N_HPP_