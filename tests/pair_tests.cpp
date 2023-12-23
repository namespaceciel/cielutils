#include <gtest/gtest.h>

#include <ciel/utility.hpp>

TEST(pair_tests, all) {

    ciel::pair<size_t, double> p5(2, 1.5);
    ciel::pair<size_t, double> p6 = p5;
    ASSERT_EQ(p5, p6);
    ASSERT_EQ(p5.first, p6.first);
    ASSERT_EQ(p5.second, p6.second);

    const ciel::pair<int, int> p7 = p6;
    ASSERT_EQ(p7.first, 2);
    ASSERT_EQ(p7.second, 1);

    // p5 and p8 have different template, move will transform to forward, p5 won't clear
    [[maybe_unused]] const ciel::pair<int, int> p8 = std::move(p5);
    ASSERT_EQ(p5.first, 2);
    ASSERT_EQ(p5.second, 1.5);

    p5 = std::move(p6);
    ASSERT_EQ(p5.first, 2);
    ASSERT_EQ(p5.second, 1.5);

    const ciel::pair p9(6, 2.2);
    const ciel::pair p10 = ciel::make_pair(6, 2.2);

    const auto i1 = p9.first;
    volatile auto i2 = p9.second;
    const ciel::pair p11 = ciel::make_pair(i1, i2);
    ASSERT_EQ(p9, p10);
    ASSERT_EQ(p9, p11);

    ciel::pair<int, float> p12(7, 9.1);
    ciel::pair<int, float> p13(12, 0.1);
    std::swap(p12, p13);
    ASSERT_EQ(p12.first, 12);
    ASSERT_EQ(p13.first, 7);
}