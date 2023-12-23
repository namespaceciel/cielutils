#ifndef CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_UNINITIALIZED_FILL_HPP_
#define CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_UNINITIALIZED_FILL_HPP_

#include <ciel/config.hpp>
#include <ciel/iterator_impl/iterator_traits.hpp>
#include <ciel/memory_impl/addressof.hpp>

NAMESPACE_CIEL_BEGIN

template<class ForwardIt, class T>
auto uninitialized_fill(ForwardIt first, ForwardIt last, const T& value) -> void {
    using V = typename iterator_traits<ForwardIt>::value_type;
    ForwardIt current = first;
    CIEL_TRY {
        for (; current != last; ++current) {
            ::new(static_cast<void *>(ciel::addressof(*current))) V(value);
        }
    } CIEL_CATCH(...) {
        for (; first != current; ++first) {
            first->~V();
        }
        CIEL_THROW;
    }
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_UNINITIALIZED_FILL_HPP_