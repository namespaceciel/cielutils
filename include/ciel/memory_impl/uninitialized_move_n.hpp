#ifndef CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_UNINITIALIZED_MOVE_N_HPP_
#define CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_UNINITIALIZED_MOVE_N_HPP_

#include <ciel/config.hpp>
#include <ciel/iterator_impl/iterator_traits.hpp>
#include <ciel/memory_impl/addressof.hpp>
#include <ciel/memory_impl/destroy.hpp>
#include <ciel/utility_impl/pair.hpp>

NAMESPACE_CIEL_BEGIN

template<class InputIt, class Size, class ForwardIt>
auto uninitialized_move_n(InputIt first, Size count, ForwardIt d_first) -> pair<InputIt, ForwardIt> {
    using Value = typename iterator_traits<ForwardIt>::value_type;
    ForwardIt current = d_first;
    CIEL_TRY {
        for (; count > 0; ++first, (void) ++current, --count) {
            ::new(const_cast<void *>(static_cast<const volatile void *>(ciel::addressof(*current)))) Value(
                    std::move(*first));
        }
    } CIEL_CATCH(...) {
        ciel::destroy(d_first, current);
        CIEL_THROW;
    }
    return {first, current};
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_UNINITIALIZED_MOVE_N_HPP_