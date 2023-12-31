#ifndef CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_HAS_MEMBER_ELEMENT_TYPE_HPP_
#define CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_HAS_MEMBER_ELEMENT_TYPE_HPP_

#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
concept has_member_element_type = requires { typename T::element_type; };

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_ITERATOR_IMPL_HAS_MEMBER_ELEMENT_TYPE_HPP_