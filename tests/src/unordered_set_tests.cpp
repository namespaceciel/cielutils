#include <gtest/gtest.h>

#include <algorithm>
#include <ciel/unordered_set.hpp>
#include <ciel/vector.hpp>
#include <random>

TEST(unordered_set_tests, large_amount_insertion_finding_deletion) {
    std::random_device rd;
    std::mt19937 g(rd());

    ciel::vector<size_t> v;
    for (size_t i = 0; i < 5000; ++i) {
        v.emplace_back(i);
    }

    for (size_t loop = 0; loop < 20; ++loop) {
        ciel::unordered_set s(v.begin(), v.end());

        std::shuffle(v.begin(), v.end(), g);

        for (const size_t i : v) {
            ASSERT_TRUE(s.contains(i));
            s.erase(i);
        }

        ASSERT_TRUE(s.empty());
    }
}