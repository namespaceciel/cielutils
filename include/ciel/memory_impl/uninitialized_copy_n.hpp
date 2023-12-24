#ifndef CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_UNINITIALIZED_COPY_N_HPP_
#define CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_UNINITIALIZED_COPY_N_HPP_

#include <ciel/config.hpp>
#include <ciel/iterator_impl/iterator_traits.hpp>
#include <ciel/memory_impl/addressof.hpp>

NAMESPACE_CIEL_BEGIN

template<class InputIt, class Size, class NoThrowForwardIt>
auto uninitialized_copy_n(InputIt first, Size count, NoThrowForwardIt d_first) -> NoThrowForwardIt {
    using T = typename iterator_traits<NoThrowForwardIt>::value_type;
    NoThrowForwardIt current = d_first;
    CIEL_TRY {
        for (; count > 0; ++first, (void) ++current, --count) {
            ::new(static_cast<void *>(ciel::addressof(*current))) T(*first);
        }
    } CIEL_CATCH (...) {
        for (; d_first != current; ++d_first) {
            d_first->~T();
        }
        CIEL_THROW;
    }
    return current;
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_UNINITIALIZED_COPY_N_HPP_