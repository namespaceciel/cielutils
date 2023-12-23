#ifndef CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_BOOLEAN_TESTABLE_HPP_
#define CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_BOOLEAN_TESTABLE_HPP_

#include <ciel/concepts_impl/convertible_to.hpp>
#include <ciel/config.hpp>

NAMESPACE_CIEL_BEGIN

template<class B>
concept boolean_testable_helper = convertible_to<B, bool>;

template<class B>
concept boolean_testable = boolean_testable_helper<B>
    && requires(B&& b) {{ !std::forward<B>(b) } -> boolean_testable_helper; };

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_CONCEPTS_IMPL_BOOLEAN_TESTABLE_HPP_