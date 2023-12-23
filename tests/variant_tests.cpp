#include <gtest/gtest.h>

#include <ciel/variant.hpp>

namespace {

struct BoolLike {
    BoolLike(bool /*unused*/) noexcept {}
    operator bool() { return true; }
};

}   // namespace

TEST(variant_tests, basic) {
    ciel::variant<int, float> v;
    ciel::variant<int, float> w;

    v = 12;
    [[maybe_unused]] const int i = ciel::get<int>(v);

    w = ciel::get<int>(v);
    w = ciel::get<0>(v);
    w = v;

#ifdef CIEL_HAS_EXCEPTIONS
    ASSERT_THROW((void)ciel::get<float>(w), std::bad_variant_access);
#endif

    ciel::variant<std::string> x("abc");
    x = "def";

    ciel::variant<std::string, const void*> y("abc");
    ASSERT_TRUE(ciel::holds_alternative<const void*>(y));

    using namespace std::literals;
    y = "xyz"s;
    ASSERT_TRUE(ciel::holds_alternative<std::string>(y));
}

TEST(variant_tests, converting_constructor) {
    // P0608R3, arithmetic narrowing options is not allowed in variant's converting constructor/assignment
    // https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0608r3.html

    const ciel::variant<std::string> v1("abc");

    const ciel::variant<std::string, const char*> v2("abc");
    ASSERT_TRUE(ciel::holds_alternative<const char*>(v2));

    const ciel::variant<std::string, bool> v3("abc");   // get rid of arithmetic narrowing options: bool
    ASSERT_TRUE(ciel::holds_alternative<std::string>(v3));

    const ciel::variant<float, long long, double> v4(0);    // get rid of arithmetic narrowing options: float and double
    ASSERT_TRUE(ciel::holds_alternative<long long>(v4));

    const ciel::variant<float, long> v5(0);
    ASSERT_TRUE(ciel::holds_alternative<long>(v5));

    const ciel::variant<short, long long> v6(0);
    ASSERT_TRUE(ciel::holds_alternative<long long>(v6));

    const ciel::variant<char, std::optional<char16_t>> v7(u'\u2043');
    ASSERT_TRUE(ciel::holds_alternative<std::optional<char16_t>>(v7));

    double d = 3.14;
    const ciel::variant<int, std::reference_wrapper<double>> v8(d);
    ASSERT_TRUE(ciel::holds_alternative<std::reference_wrapper<double>>(v8));

    const ciel::variant<bool, BoolLike> v9(true);
    ASSERT_TRUE(ciel::holds_alternative<bool>(v9));

    const ciel::variant<BoolLike, int> v10(true);
    ASSERT_TRUE(ciel::holds_alternative<int>(v10));

    const ciel::variant<BoolLike, std::string> v11(true);
    ASSERT_TRUE(ciel::holds_alternative<BoolLike>(v11));

    using namespace std::literals;
    const ciel::variant<const char*, std::string_view> v12("abc"s);
    ASSERT_TRUE(ciel::holds_alternative<std::string_view>(v12));

    // only inherit one std::string converting function, won't yield errors
    const ciel::variant<int, std::string, std::string> v13(1);
    ASSERT_TRUE(ciel::holds_alternative<int>(v13));
}