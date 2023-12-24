#include <gtest/gtest.h>

#include <algorithm>
#include <ciel/set.hpp>
#include <ciel/vector.hpp>
#include <random>

TEST(set_tests, constructors_and_assignments) {
    const ciel::set<int> s0;

    const ciel::set s1{0, 1, 2, 3, 4, 5};
    const ciel::set s2{5, 3, 1, 4, 0, 2};

    const ciel::vector v1{5, 1, 4, 0, 2, 3};
    const ciel::set<int> s3(v1.begin(), v1.end());

    const ciel::set s4{0, 2, 3, 2, 1, 4, 1, 2, 4, 0, 5, 0, 3, 1, 0, 5, 0, 5, 3, 0, 2, 1, 3, 4};

    const ciel::vector v2{5, 1, 3, 1, 4, 0, 5, 1, 3, 0, 4, 5, 0, 0, 2, 3, 1, 4, 4, 0, 2, 3};
    ciel::set<int> s5(v2.begin(), v2.end());

    ciel::set s6(s5);

    ASSERT_TRUE(s0.empty());
    ASSERT_EQ(s0.size(), 0);

    ASSERT_EQ(s1.size(), 6);
    ASSERT_EQ(s2.size(), 6);
    ASSERT_EQ(s3.size(), 6);
    ASSERT_EQ(s4.size(), 6);
    ASSERT_EQ(s5.size(), 6);
    ASSERT_EQ(s6.size(), 6);

    int i = 0;
    for (auto it = s1.begin(); it != s1.end(); ++it) {
        ASSERT_EQ(*it, i++);
    }

    i = 5;
    for (auto it = s1.rbegin(); it != s1.rend(); ++it) {
        ASSERT_EQ(*it, i--);
    }

    ASSERT_EQ(s1, s2);
    ASSERT_EQ(s1, s3);
    ASSERT_EQ(s1, s4);
    ASSERT_EQ(s1, s5);
    ASSERT_EQ(s1, s6);

    ciel::set<int> s7(std::move(s6));
    ASSERT_TRUE(s6.empty());
    ASSERT_EQ(s1, s7);

    s5 = s7;
    ASSERT_EQ(s1, s5);

    s5 = s6;
    ASSERT_TRUE(s5.empty());

    s5 = std::move(s7);
    ASSERT_EQ(s1, s5);
    ASSERT_TRUE(s7.empty());

    s5 = {2, 1, 5, 3, 0, 4, 0, 1, 2, 3, 4, 5, 0, 2, 1, 3, 5, 4, 0, 1, 2, 3, 4, 5, 2, 1, 3, 4, 1, 2, 5, 0};
    ASSERT_EQ(s1, s5);

    s7.swap(s5);
    ASSERT_TRUE(s5.empty());
    ASSERT_EQ(s1, s7);
}

TEST(set_tests, insert_and_delete) {
    ciel::set<int> s1;
    s1.insert({0, 2, 1, 3, 4, 0, 1, 2, 3, 4, 5, 0, 2, 1, 3, 5, 4, 0, 6, 2, 3, 5, 4, 0, 2, 1, 3, 5, 4, 0, 2, 1, 3, 4, 5,
               5, 0, 2, 1, 3, 4, 5, 2, 3, 0, 5, 4, 1, 2});
    s1.emplace_hint(s1.end(), 7);
    s1.emplace_hint(s1.begin(), 8);
    s1.emplace(1);
    s1.emplace(10);
    s1.emplace_hint(s1.end(), 9);

    ASSERT_EQ(s1, ciel::set({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}));

    s1.erase(5);
    s1.erase(0);
    s1.erase(8);

    ASSERT_EQ(s1, ciel::set({1, 2, 3, 4, 6, 7, 9, 10}));

    ciel::vector v1{8, 0, 5};
    s1.insert(v1.begin(), v1.end());
    s1.insert(12);
    s1.insert(11);

    ASSERT_EQ(s1, ciel::set({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}));

    s1.clear();
    ASSERT_TRUE(s1.empty());
}

TEST(set_tests, find) {
    ciel::set s1 {3, 1, 0, 4, 5, 7, 0, 3, 1, 2, 4, 0, 1, 0, 3, 8, 4, 6, 5, 3, 4, 7, 0, 3, 1, 4, 0, 7, 4, 1, 9, 5, 9, 1,
                  5, 1, 2, 9, 4, 1, 4};
    ASSERT_EQ(*s1.find(3), 3);
    ASSERT_TRUE(s1.contains(9));
    ASSERT_EQ(s1.count(0), 1);
    ASSERT_EQ(*s1.lower_bound(4), 4);
    ASSERT_EQ(*s1.upper_bound(7), 8);

    auto er = s1.equal_range(5);
    ASSERT_EQ(ciel::distance(er.first, er.second), 1);
}

TEST(set_tests, sort) {
    std::random_device rd;
    std::mt19937 g(rd());

    ciel::vector<size_t> v;
    for (size_t i = 0; i < 5000; ++i) {
        v.emplace_back(i);
    }
    for (size_t loop = 0; loop < 20; ++loop) {
        std::shuffle(v.begin(), v.end(), g);

        ciel::set s(v.begin(), v.end());

        ASSERT_TRUE(ciel::is_sorted(s.begin(), s.end()));
    }
}

TEST(set_tests, sorted_each_insert) {
    // insert sorted range to check balance performance
    for (size_t loop = 0; loop < 20; ++loop) {
        ciel::set<size_t> s;
        for (size_t i = 0; i < 2500; ++i) {
            s.emplace(i);
        }
        for (size_t i = 7500; i > 2499; --i) {
            s.emplace(i);
        }
        for (size_t i = 7501; i < 10000; ++i) {
            s.emplace(i);
        }

        ASSERT_TRUE(ciel::is_sorted(s.begin(), s.end()));
    }
}

TEST(set_tests, large_amount_random_deletion) {
    std::random_device rd;
    std::mt19937 g(rd());

    ciel::vector<size_t> v;
    for (size_t i = 0; i < 5000; ++i) {
        v.emplace_back(i);
    }

    for (size_t loop = 0; loop < 20; ++loop) {
        ciel::set s(v.begin(), v.end());

        std::shuffle(v.begin(), v.end(), g);
        for (const size_t i : v) {
            s.erase(i);
        }

        ASSERT_TRUE(s.empty());
    }
}