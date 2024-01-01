#include <gtest/gtest.h>

#include <ciel/tuple.hpp>

namespace {
    
struct ConstructAndAssignCounter {
    inline static size_t copy = 0;
    inline static size_t move = 0;

    ConstructAndAssignCounter() noexcept = default;
    ConstructAndAssignCounter(const ConstructAndAssignCounter& /*unused*/) noexcept { ++copy; }
    ConstructAndAssignCounter(ConstructAndAssignCounter&& /*unused*/) noexcept { ++move; }
    auto operator=(const ConstructAndAssignCounter& /*unused*/) noexcept -> ConstructAndAssignCounter& {
        ++copy;
        return *this;
    }
    auto operator=(ConstructAndAssignCounter&& /*unused*/) noexcept -> ConstructAndAssignCounter& {
        ++move;
        return *this;
    }
};

}   // namespace

TEST(tuple_tests, move_behavior) {
    ConstructAndAssignCounter::move = 0;

    ciel::tuple<ConstructAndAssignCounter, ConstructAndAssignCounter> t5{};
    ciel::tuple<ConstructAndAssignCounter, ConstructAndAssignCounter> t6{};
    t5 = std::move(t6);
    ASSERT_EQ(ConstructAndAssignCounter::move, 2);

    t5.swap(t6);
    ASSERT_EQ(ConstructAndAssignCounter::move, 8);
}

TEST(tuple_tests, constructors) {
    [[maybe_unused]] constexpr ciel::tuple t0;
    ciel::tuple<int, size_t, char> t1;
    ciel::tuple<int, size_t, char> t2(-1, 10, 'c');
    const ciel::tuple t3(-1, 10UL, 'c');
    ASSERT_EQ(t2, t3);

    t1.swap(t2);
    ASSERT_EQ(t1, t3);

    t2 = t1;
    ASSERT_EQ(t2, t3);

    ciel::pair p1(-5, 't');
    ciel::tuple t4(p1);
    ASSERT_EQ(t4, ciel::tuple(-5, 't'));

    t4 = ciel::pair(-10, 'a');
    ASSERT_EQ(t4, ciel::tuple(-10, 'a'));

    t4 = std::move(p1);
    ASSERT_EQ(t4, ciel::tuple(-5, 't'));

    [[maybe_unused]] const ciel::tuple t5(std::move(p1));
}

TEST(tuple_tests, compare_three_way) {
    ciel::tuple<int, long, unsigned long> t1{0, 1, 2};
    const ciel::tuple<short, int, unsigned int> t2{0, 1, 5};
    ASSERT_TRUE(t1 < t2);

    std::get<0>(t1) = 1;
    ASSERT_TRUE(t1 > t2);
}