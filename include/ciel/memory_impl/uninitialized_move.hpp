#ifndef CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_UNINITIALIZED_MOVE_HPP_
#define CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_UNINITIALIZED_MOVE_HPP_

#include <ciel/config.hpp>
#include <ciel/iterator_impl/iterator_traits.hpp>
#include <ciel/memory_impl/addressof.hpp>
#include <ciel/memory_impl/destroy.hpp>

NAMESPACE_CIEL_BEGIN

template<class InputIt, class NoThrowForwardIt>
auto uninitialized_move(InputIt first, InputIt last, NoThrowForwardIt d_first) -> NoThrowForwardIt {
    using Value = typename iterator_traits<NoThrowForwardIt>::value_type;
    NoThrowForwardIt current = d_first;
    CIEL_TRY {
        for (; first != last; ++first, (void) ++current) {
            ::new(static_cast<void *>(ciel::addressof(*current))) Value(std::move(*first));
        }
        return current;
    } CIEL_CATCH (...) {
        ciel::destroy(d_first, current);
        CIEL_THROW;
    }
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_UNINITIALIZED_MOVE_HPP_