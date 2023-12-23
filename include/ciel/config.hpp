#ifndef CIELUTILS_INCLUDE_CIEL_CONFIG_HPP_
#define CIELUTILS_INCLUDE_CIEL_CONFIG_HPP_

#include <exception>

#ifdef __cpp_exceptions
#define CIEL_HAS_EXCEPTIONS
#endif

#ifdef CIEL_HAS_EXCEPTIONS
#define CIEL_TRY      try
#define CIEL_CATCH(X) catch (X)
#define CIEL_THROW    throw
#else
#define CIEL_TRY      if constexpr (true)
#define CIEL_CATCH(X) if constexpr (false)
#define CIEL_THROW
#endif

#ifdef __cpp_rtti
#define CIEL_HAS_RTTI
#endif

#define NAMESPACE_CIEL_BEGIN namespace ciel { inline namespace ver1 {

#define NAMESPACE_CIEL_END }}

NAMESPACE_CIEL_BEGIN

[[noreturn]] inline void unreachable() {
    __builtin_unreachable();
}

template<class Exception>
[[noreturn]] inline void THROW([[maybe_unused]] Exception&& e) {
#ifdef CIEL_HAS_EXCEPTIONS
    throw e;
#else
    std::terminate();
#endif
}

NAMESPACE_CIEL_END

#endif // CIELUTILS_INCLUDE_CIEL_CONFIG_HPP_