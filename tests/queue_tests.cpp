#include <gtest/gtest.h>

#include <algorithm>
#include <ciel/algorithm.hpp>
#include <ciel/functional.hpp>
#include <ciel/queue.hpp>
#include <random>

TEST(queue_tests, priority_queue) {
    ciel::priority_queue<int> pq;
    ASSERT_TRUE(pq.empty());
    ASSERT_EQ(pq.size(), 0);

    pq.push(1);
    ASSERT_EQ(pq.top(), 1);

    pq.emplace(5);
    pq.push(5);
    ASSERT_EQ(pq.size(), 3);
    ASSERT_EQ(pq.top(), 5);
    ASSERT_TRUE(ciel::is_heap(pq.container().begin(), pq.container().end()));

    pq.pop();
    ASSERT_EQ(pq.size(), 2);
    ASSERT_EQ(pq.top(), 5);
}

TEST(queue_tests, priority_queue2) {
    std::random_device rd;
    std::mt19937 g(rd());

    ciel::vector<size_t> v;
    for (size_t i = 0; i < 5000; ++i) {
        v.emplace_back(i);
    }
    for (size_t loop = 0; loop < 20; ++loop) {
        std::shuffle(v.begin(), v.end(), g);

        ciel::priority_queue<size_t> pq(ciel::less<size_t>(), std::move(v));
        ASSERT_TRUE(v.empty());
        ASSERT_TRUE(ciel::is_heap(pq.container().begin(), pq.container().end()));

        while (!pq.empty()) {
            v.emplace_back(pq.top());
            pq.pop();
        }

        for (size_t i = 0, j = 4999; i < 5000; ++i, --j) {
            ASSERT_EQ(v[i], j);
        }
    }
}

TEST(queue_tests, priority_queue3) {
    std::random_device rd;
    std::mt19937 g(rd());

    ciel::vector<size_t> v;
    for (size_t i = 0; i < 5000; ++i) {
        v.emplace_back(i);
    }
    for (size_t loop = 0; loop < 20; ++loop) {
        std::shuffle(v.begin(), v.end(), g);

        ciel::make_heap(v.begin(), v.end());
        ciel::sort_heap(v.begin(), v.end());

        for (size_t i = 0; i < 5000; ++i) {
            ASSERT_EQ(v[i], i);
        }
    }
}