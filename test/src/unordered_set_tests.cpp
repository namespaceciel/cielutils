#include <gtest/gtest.h>

#include <algorithm>
#include <ciel/unordered_set.hpp>
#include <ciel/vector.hpp>
#include <random>

TEST(unordered_set_tests, constructors_and_assignments) {
    const ciel::unordered_set<int> s1;
    ASSERT_TRUE(s1.empty());
    ASSERT_EQ(s1.size(), 0);

    const ciel::unordered_set s2({0, 1, 2, 3, 4, 5});
    ASSERT_EQ(s2.size(), 6);
    ASSERT_TRUE(s2.contains(0));
    ASSERT_TRUE(s2.contains(1));
    ASSERT_TRUE(s2.contains(2));
    ASSERT_TRUE(s2.contains(3));
    ASSERT_TRUE(s2.contains(4));
    ASSERT_TRUE(s2.contains(5));

    ciel::unordered_set s3(s2);
    ASSERT_EQ(s3.size(), 6);
    ASSERT_TRUE(s3.contains(0));
    ASSERT_TRUE(s3.contains(1));
    ASSERT_TRUE(s3.contains(2));
    ASSERT_TRUE(s3.contains(3));
    ASSERT_TRUE(s3.contains(4));
    ASSERT_TRUE(s3.contains(5));

    ciel::unordered_set s4(std::move(s3));
    ASSERT_TRUE(s3.empty());
    ASSERT_EQ(s4.size(), 6);
    ASSERT_TRUE(s4.contains(0));
    ASSERT_TRUE(s4.contains(1));
    ASSERT_TRUE(s4.contains(2));
    ASSERT_TRUE(s4.contains(3));
    ASSERT_TRUE(s4.contains(4));
    ASSERT_TRUE(s4.contains(5));

    s3 = s4;
    ASSERT_EQ(s3.size(), 6);
    ASSERT_TRUE(s3.contains(0));
    ASSERT_TRUE(s3.contains(1));
    ASSERT_TRUE(s3.contains(2));
    ASSERT_TRUE(s3.contains(3));
    ASSERT_TRUE(s3.contains(4));
    ASSERT_TRUE(s3.contains(5));

    s3.clear();
    ASSERT_TRUE(s3.empty());

    s3 = std::move(s4);
    ASSERT_TRUE(s4.empty());
    ASSERT_EQ(s3.size(), 6);
    ASSERT_TRUE(s3.contains(0));
    ASSERT_TRUE(s3.contains(1));
    ASSERT_TRUE(s3.contains(2));
    ASSERT_TRUE(s3.contains(3));
    ASSERT_TRUE(s3.contains(4));
    ASSERT_TRUE(s3.contains(5));
}

TEST(unordered_set_tests, find) {
    const ciel::unordered_set s1({0, 253, 16, 412, 687, 311, 987});

    ASSERT_EQ(s1.find(10), s1.end());
    ASSERT_EQ(s1.find(167), s1.end());
    ASSERT_EQ(*s1.find(0), 0);
    ASSERT_EQ(*s1.find(253), 253);
    ASSERT_EQ(*s1.find(16), 16);
    ASSERT_EQ(*s1.find(412), 412);
    ASSERT_EQ(*s1.find(687), 687);
    ASSERT_EQ(*s1.find(311), 311);
    ASSERT_EQ(*s1.find(987), 987);
}

TEST(unordered_set_tests, insert) {
    ciel::unordered_set s1({0, 253, 16, 412, 687, 311, 987});

    const auto res1 = s1.insert(0);
    ASSERT_FALSE(res1.second);
    ASSERT_EQ(*res1.first, 0);

    const auto res2 = s1.insert(412);
    ASSERT_FALSE(res2.second);
    ASSERT_EQ(*res2.first, 412);

    const auto res3 = s1.insert(12);
    ASSERT_TRUE(res3.second);
    ASSERT_EQ(*res3.first, 12);

    const auto res4 = s1.insert(12);
    ASSERT_FALSE(res4.second);
    ASSERT_EQ(*res4.first, 12);

    s1.insert({-1, 0, 1, 2});
    ASSERT_TRUE(s1.contains(-1));
    ASSERT_TRUE(s1.contains(1));
    ASSERT_TRUE(s1.contains(2));
}

TEST(unordered_set_tests, erase) {
    ciel::unordered_set s1({0, 253, 16, 412, 687, 311, 987, 3123, 1231, 1345, 34, 56, 7, 5, 67, 63, 4, 8, 74});

    ASSERT_EQ(s1.erase(16), 1);
    ASSERT_EQ(s1.erase(1116), 0);
    ASSERT_EQ(s1.erase(8), 1);
    ASSERT_EQ(s1.erase(1231), 1);
    ASSERT_EQ(s1.erase(4), 1);
    ASSERT_TRUE(s1.contains(*s1.erase(s1.begin())));
}

TEST(unordered_set_tests, large_amount_insertion_finding_deletion) {
    std::random_device rd;
    std::mt19937 g(rd());

    ciel::vector<size_t> v;
    for (size_t i = 0; i < 5000; ++i) {
        v.emplace_back(i);
    }

    for (size_t loop = 0; loop < 20; ++loop) {
        ciel::unordered_set s(v.begin(), v.end());

        std::ranges::shuffle(v, g);

        for (const size_t i : v) {
            ASSERT_TRUE(s.contains(i));
            s.erase(i);
        }

        ASSERT_TRUE(s.empty());
    }
}