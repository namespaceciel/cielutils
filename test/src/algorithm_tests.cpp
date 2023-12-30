#include <gtest/gtest.h>

#include <algorithm>
#include <ciel/algorithm.hpp>
#include <ciel/map.hpp>
#include <ciel/vector.hpp>
#include <random>

TEST(algorithm_tests, partial_sort) {
    std::random_device rd;
    std::mt19937 g(rd());

    ciel::vector<size_t> v;
    for (size_t i = 0; i < 5000; ++i) {
        v.emplace_back(i);
    }

    for (size_t loop = 0; loop < 20; ++loop) {
        std::shuffle(v.begin(), v.end(), g);

        ciel::partial_sort(v.begin(), v.begin() + 3426, v.end());
        ASSERT_TRUE(ciel::is_sorted(v.begin(), v.begin() + 3426));

        ciel::partial_sort(v.begin(), v.end(), v.end());
        ASSERT_TRUE(ciel::is_sorted(v.begin(), v.end()));
    }

    for (size_t i = 0; i < 5000; ++i) {
        v.emplace_back(i);
    }

    for (size_t loop = 0; loop < 20; ++loop) {
        std::shuffle(v.begin(), v.end(), g);

        ciel::partial_sort(v.begin(), v.begin() + 7426, v.end());
        ASSERT_TRUE(ciel::is_sorted(v.begin(), v.begin() + 7426));

        ciel::partial_sort(v.begin(), v.end(), v.end());
        ASSERT_TRUE(ciel::is_sorted(v.begin(), v.end()));
    }
}

TEST(algorithm_tests, sort) {
    std::random_device rd;
    std::mt19937 g(rd());

    ciel::vector<size_t> v;
    for (size_t i = 0; i < 5000; ++i) {
        v.emplace_back(i);
    }

    for (size_t loop = 0; loop < 20; ++loop) {
        std::shuffle(v.begin(), v.end(), g);

        ciel::sort(v.begin(), v.end());
        ASSERT_TRUE(ciel::is_sorted(v.begin(), v.end()));
    }

    for (size_t i = 0; i < 5000; ++i) {
        v.emplace_back(i);
    }

    for (size_t loop = 0; loop < 20; ++loop) {
        std::shuffle(v.begin(), v.end(), g);

        ciel::sort(v.begin(), v.end());
        ASSERT_TRUE(ciel::is_sorted(v.begin(), v.end()));
    }
}

TEST(algorithm_tests, stable_sort) {
    std::random_device rd;
    std::mt19937 g(rd());

    ciel::vector<size_t> v;
    for (size_t i = 0; i < 5000; ++i) {
        v.emplace_back(i);
    }

    for (size_t loop = 0; loop < 20; ++loop) {
        std::shuffle(v.begin(), v.end(), g);

        ciel::stable_sort(v.begin(), v.end());
        ASSERT_TRUE(ciel::is_sorted(v.begin(), v.end()));
    }

    for (size_t i = 0; i < 5000; ++i) {
        v.emplace_back(i);
    }

    for (size_t loop = 0; loop < 20; ++loop) {
        std::shuffle(v.begin(), v.end(), g);

        ciel::stable_sort(v.begin(), v.end());
        ASSERT_TRUE(ciel::is_sorted(v.begin(), v.end()));
    }
}

TEST(algorithm_tests, stable_sort_stability) {
    // To test stability of stable_sort, we shuffle the ranges full of duplicate elements,
    // make pairs with each one of them and their index, then stable_sort,
    // and use map to mark the index of same elements, checking whether the indexs are increasing

    std::random_device rd;
    std::mt19937 g(rd());

    ciel::vector<size_t> v;
    for (size_t i = 0; i < 1000; ++i) {
        v.emplace_back(i);
        v.emplace_back(i);
        v.emplace_back(i);
        v.emplace_back(i);
        v.emplace_back(i);
    }

    for (size_t loop = 0; loop < 10; ++loop) {
        std::shuffle(v.begin(), v.end(), g);

        ciel::vector<ciel::pair<size_t, size_t>> vp;
        for (size_t i = 0; i < v.size(); ++i) {
            vp.emplace_back(v[i], i);
        }

        ciel::stable_sort(vp.begin(), vp.end(),
                          [](const ciel::pair<size_t, size_t>& a, const ciel::pair<size_t, size_t>& b) {
            return a.first < b.first;
        });

        ASSERT_TRUE(ciel::is_sorted(vp.begin(), vp.end(),
                                    [](const ciel::pair<size_t, size_t>& a, const ciel::pair<size_t, size_t>& b) {
            return a.first < b.first;
        }));

        ciel::map<size_t, size_t> map;
        for (const auto& [first, last] : vp) {
            if (auto it = map.find(first); it == map.end()) {
                map.emplace(first, last);
            } else {
                ASSERT_TRUE(it->second < last);
                it->second = last;
            }
        }
    }
}