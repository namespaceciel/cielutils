#ifndef CIELUTILS_INCLUDE_CIEL_UTILITY_IMPL_INTEGER_SEQUENCE_HPP_
#define CIELUTILS_INCLUDE_CIEL_UTILITY_IMPL_INTEGER_SEQUENCE_HPP_

#include <ciel/config.hpp>
#include <ciel/type_traits_impl/is_integral.hpp>
#include <cstddef>

NAMESPACE_CIEL_BEGIN

template<class T, T... Ints>
    requires is_integral_v<T>
struct integer_sequence {
    using value_type = T;

    static constexpr auto size() noexcept -> size_t {
        return sizeof...(Ints);
    }
};

template<size_t... Ints>
using index_sequence = integer_sequence<size_t, Ints...>;

namespace details {

// https://stackoverflow.com/questions/17424477/implementation-c14-make-integer-sequence

template<class, class>
struct merge_and_renumber;

template<size_t... I1, size_t... I2>
struct merge_and_renumber<index_sequence<I1...>, index_sequence<I2...>> {
    using type = index_sequence<I1..., (sizeof...(I1) + I2)...>;
};

template<size_t N>
struct make_index_sequence_helper : merge_and_renumber<typename make_index_sequence_helper<N / 2>::type,
                                                       typename make_index_sequence_helper<N - N / 2>::type> {};

template<>
struct make_index_sequence_helper<0> {
    using type = index_sequence<>;
};

template<>
struct make_index_sequence_helper<1> {
    using type = index_sequence<0>;
};

}   // namespace details

template<size_t N>
using make_index_sequence = typename details::make_index_sequence_helper<N>::type;

namespace details {

template<typename T, size_t N, typename Indices = make_index_sequence<N>>
struct make_integer_sequence_helper;

template<typename T, size_t N, size_t... Indices>
struct make_integer_sequence_helper<T, N, index_sequence<Indices...>> {
    using type = integer_sequence<T, static_cast<T>(Indices)...>;
};

}   // namespace details

template<class T, T N>
using make_integer_sequence = typename details::make_integer_sequence_helper<T, N>::type;

template<class... T>
using index_sequence_for = make_index_sequence<sizeof...(T)>;

// The following is deprecated, used to serve for tuple constructors
template<class...>
struct tuple_types {};

namespace details {

template<class, size_t>
struct make_tuple_indices_helper;

template<size_t... I, size_t R>
struct make_tuple_indices_helper<index_sequence<I...>, R> {
    using type = index_sequence<(R + I)...>;
};

template<class, class, size_t>
struct make_tuple_types_back_helper;

template<class... Needed, class... Dumped, size_t S>
    requires (S == 0)
struct make_tuple_types_back_helper<tuple_types<Needed...>, tuple_types<Dumped...>, S> {
    using type = tuple_types<Needed...>;
};

template<class... Needed, class Front, class... Dumped, size_t S>
    requires (S > 0)
struct make_tuple_types_back_helper<tuple_types<Needed...>, tuple_types<Front, Dumped...>, S>
    : make_tuple_types_back_helper<tuple_types<Needed..., Front>, tuple_types<Dumped...>, S - 1> {};

template<class, size_t>
struct make_tuple_types_front_helper;

template<class... Needed, size_t R>
    requires (R == 0)
struct make_tuple_types_front_helper<tuple_types<Needed...>, R> {
    using type = tuple_types<Needed...>;
};

template<class Front, class... Needed, size_t R>
    requires (R > 0)
struct make_tuple_types_front_helper<tuple_types<Front, Needed...>, R>
    : make_tuple_types_front_helper<tuple_types<Needed...>, R - 1> {};

}   // namespace details

template<size_t S, size_t R = 0>
using make_tuple_indices = typename details::make_tuple_indices_helper<make_index_sequence<S - R>, R>::type;

template<class, size_t, size_t = 0>
struct make_tuple_types;

template<class... Types, size_t S, size_t R>
struct make_tuple_types<tuple_types<Types...>, S, R> {
private:
    using first_step = typename details::make_tuple_types_back_helper<tuple_types<>, tuple_types<Types...>, S>::type;
public:
    using type = typename details::make_tuple_types_front_helper<first_step, R>::type;
};

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_UTILITY_IMPL_INTEGER_SEQUENCE_HPP_