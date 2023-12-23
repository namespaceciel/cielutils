#ifndef CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_NEXT_HPP_
#define CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_NEXT_HPP_

#include <ciel/config.hpp>
#include <ciel/iterator_impl/advance.hpp>

NAMESPACE_CIEL_BEGIN

template<class InputIt>
constexpr auto next(InputIt it, typename iterator_traits<InputIt>::difference_type n = 1) -> InputIt {
    ciel::advance(it, n);
    return it;
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_NEXT_HPP_