#include <gtest/gtest.h>

#include <ciel/array.hpp>

TEST(array_tests, all) {
    ciel::array a = {0, 1, 2, 3, 4};
    ciel::array b = {4, 3, 2, 1, 0};
    ciel::array c = b;

    for (size_t i = 0; i < a.size(); ++i) {
        ASSERT_EQ(a.at(i), i);
        ASSERT_EQ(a[i], i);
        ASSERT_EQ(a.data()[i], i);
    }

    ASSERT_EQ(a.front(), 0);
    ASSERT_EQ(a.back(), 4);

    ASSERT_EQ(b.front(), 4);
    ASSERT_EQ(b.back(), 0);

    ASSERT_EQ(c.front(), 4);
    ASSERT_EQ(c.back(), 0);

    auto ab = a.begin();
    auto brb = b.rbegin();
    for (; ab != a.end() && brb != b.rend(); ++ab, ++brb) {
        ASSERT_EQ(*ab, *brb);
    }

    auto ae = a.end() - 1;
    auto bre = b.rend() - 1;
    for (; ae != a.begin() && bre != b.rbegin(); --ae, --bre) {
        ASSERT_EQ(*ae, *bre);
    }

    for (auto bb = b.begin(), cb = c.begin(); bb != b.end() && cb != c.end(); ++bb, ++cb) {
        ASSERT_EQ(*bb, *cb);
    }

    ASSERT_FALSE(a.empty());
    ASSERT_FALSE(b.empty());
    ASSERT_FALSE(c.empty());

    ASSERT_EQ(a.size(), 5);
    ASSERT_EQ(b.size(), 5);
    ASSERT_EQ(c.size(), 5);

    ASSERT_EQ(a.max_size(), 5);
    ASSERT_EQ(b.max_size(), 5);
    ASSERT_EQ(c.max_size(), 5);

    ASSERT_EQ(b, c);

    for (size_t i = 0; i < b.size(); ++i) {
        ASSERT_EQ(b[i], c[i]);
        ASSERT_EQ(b.data()[i], c.data()[i]);
    }

    ASSERT_EQ(std::get<1>(a), std::get<3>(b));
    ASSERT_EQ(std::get<2>(b), std::get<2>(c));

    std::swap(a, b);
    ASSERT_EQ(a, c);

    c.fill(10);
    for (const int i : c) {
        ASSERT_EQ(i, 10);
    }
}