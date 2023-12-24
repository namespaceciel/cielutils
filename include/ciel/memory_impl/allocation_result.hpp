#ifndef CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_ALLOCATION_RESULT_HPP_
#define CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_ALLOCATION_RESULT_HPP_

#include <ciel/config.hpp>
#include <cstddef>

NAMESPACE_CIEL_BEGIN

template<class Pointer, class SizeType = size_t>
struct allocation_result {
    Pointer ptr;
    SizeType count;
};

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_MEMORY_IMPL_ALLOCATION_RESULT_HPP_