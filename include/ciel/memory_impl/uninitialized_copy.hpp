#ifndef CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_UNINITIALIZED_COPY_HPP_
#define CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_UNINITIALIZED_COPY_HPP_

#include <ciel/config.hpp>
#include <ciel/iterator_impl/iterator_traits.hpp>
#include <ciel/memory_impl/addressof.hpp>

NAMESPACE_CIEL_BEGIN

template<class InputIt, class NoThrowForwardIt>
auto uninitialized_copy(InputIt first, InputIt last, NoThrowForwardIt d_first) -> NoThrowForwardIt {
    using T = typename iterator_traits<NoThrowForwardIt>::value_type;
    NoThrowForwardIt current = d_first;
    CIEL_TRY {
        for (; first != last; ++first, (void) ++current) {
            ::new(static_cast<void *>(ciel::addressof(*current))) T(*first);
        }
        return current;
    } CIEL_CATCH(...) {
        for (; d_first != current; ++d_first) {
            d_first->~T();
        }
        CIEL_THROW;
    }
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_UNINITIALIZED_COPY_HPP_