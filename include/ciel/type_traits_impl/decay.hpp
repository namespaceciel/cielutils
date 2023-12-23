#ifndef CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_DECAY_HPP_
#define CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_DECAY_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/add_pointer.hpp>
#include <ciel/type_traits_impl/conditional.hpp>
#include <ciel/type_traits_impl/is_array.hpp>
#include <ciel/type_traits_impl/is_function.hpp>
#include <ciel/type_traits_impl/remove_cvref.hpp>
#include <ciel/type_traits_impl/remove_extent.hpp>

NAMESPACE_CIEL_BEGIN

template<class T>
class decay {
private:
    using U = remove_reference_t<T>;

public:
    using type = conditional_t<is_array_v<U>,
                                     remove_extent_t<U>*,
                                     conditional_t<is_function_v<U>,
                                                         add_pointer_t<U>,
                                                         remove_cv_t<U>>>;
};

template<class T>
using decay_t = typename decay<T>::type;

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_TYPE_TRAITS_IMPL_DECAY_HPP_