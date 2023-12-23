#ifndef CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_ITERATOR_TAG_HPP_
#define CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_ITERATOR_TAG_HPP_

#include <ciel/config.hpp>
#include <iterator>

NAMESPACE_CIEL_BEGIN

using input_iterator_tag = std::input_iterator_tag;

using output_iterator_tag = std::output_iterator_tag;

using forward_iterator_tag = std::forward_iterator_tag;

using bidirectional_iterator_tag = std::bidirectional_iterator_tag;

using random_access_iterator_tag = std::random_access_iterator_tag;

using contiguous_iterator_tag = std::contiguous_iterator_tag;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_ITERATOR_TAG_HPP_