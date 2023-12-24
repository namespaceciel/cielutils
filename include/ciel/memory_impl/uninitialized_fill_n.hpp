#ifndef CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_UNINITIALIZED_FILL_N_HPP_
#define CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_UNINITIALIZED_FILL_N_HPP_

#include <ciel/config.hpp>
#include <ciel/iterator_impl/iterator_traits.hpp>
#include <ciel/memory_impl/addressof.hpp>

NAMESPACE_CIEL_BEGIN

template<class ForwardIt, class Size, class T>
auto uninitialized_fill_n(ForwardIt first, Size count, const T& value) -> ForwardIt {
    using V = typename iterator_traits<ForwardIt>::value_type;
    ForwardIt current = first;
    CIEL_TRY {
        for (; count > 0; ++current, (void) --count) {
            ::new(static_cast<void *>(ciel::addressof(*current))) V(value);
        }
        return current;
    } CIEL_CATCH (...) {
        for (; first != current; ++first) {
            first->~V();
        }
        CIEL_THROW;
    }
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_UNINITIALIZED_FILL_N_HPP_