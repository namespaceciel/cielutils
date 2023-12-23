#include <gtest/gtest.h>

#include <ciel/deque.hpp>
#include <ciel/functional.hpp>

namespace {

void test1() noexcept {}

int test2(double /*unused*/, float /*unused*/, long /*unused*/) noexcept {
    return 1;
}

}   // namespace

TEST(function_tests, all) {
    const ciel::function<void()> f0{nullptr};

#ifdef CIEL_HAS_EXCEPTIONS
    ASSERT_THROW(f0(), std::bad_function_call);
#endif

    const ciel::function f1{test1};
    const ciel::function f2{test2};
    ciel::function f3{[] {}};

    int i = 1;
    ciel::function f4{[&] { return i; }};

    const ciel::function f5{f4};
    ciel::function f6{std::move(f4)};

    f4 = nullptr;
    f4 = [] { return 1; };
    f4 = f5;
    f4 = std::move(f6);

    ASSERT_FALSE(ciel::is_small_object<ciel::deque<int>>::value);
    const ciel::deque<int> deque;
    ciel::function f7{[deque] { (void)deque; }};

    f3 = f7;
    f3 = f0;
    f3 = f1;

    std::swap(f3, f7);

    f7 = std::move(f3);
    ASSERT_FALSE(f3.operator bool());
}