#include <gtest/gtest.h>

#include <ciel/vector.hpp>

namespace {

struct ConstructAndAssignCounter {
    inline static size_t copy = 0;
    inline static size_t move = 0;

    ConstructAndAssignCounter() noexcept = default;
    ConstructAndAssignCounter(const ConstructAndAssignCounter&) noexcept { ++copy; }
    ConstructAndAssignCounter(ConstructAndAssignCounter&&) noexcept { ++move; }
    ConstructAndAssignCounter& operator=(const ConstructAndAssignCounter&) noexcept {
        ++copy;
        return *this;
    }
    ConstructAndAssignCounter& operator=(ConstructAndAssignCounter&&) noexcept {
        ++move;
        return *this;
    }
};

}   // namespace

TEST(vector_tests, constructors) {
    constexpr ciel::vector<int> v1;
    ASSERT_TRUE(v1.empty());
    ASSERT_EQ(v1.size(), 0);
    ASSERT_EQ(v1.capacity(), 0);
}

TEST(vector_tests, resize) {
    ciel::vector<int> v1(10);
    ASSERT_EQ(v1.size(), 10);
    for (const int i : v1) {
        ASSERT_EQ(i, 0);
    }
    for (auto it = v1.begin(); it != v1.end(); ++it) {
        ASSERT_EQ(*it, 0);
    }
    for (auto it = v1.rbegin(); it != v1.rend(); ++it) {
        ASSERT_EQ(*it, 0);
    }
    v1.clear();
    ASSERT_TRUE(v1.empty());

    ciel::vector v2(10, 5);
    ASSERT_EQ(v2.size(), 10);
    for (const int i : v2) {
        ASSERT_EQ(i, 5);
    }
    for (auto it = v2.begin(); it != v2.end(); ++it) {
        ASSERT_EQ(*it, 5);
    }
    for (auto it = v2.rbegin(); it != v2.rend(); ++it) {
        ASSERT_EQ(*it, 5);
    }
    v1 = v2;
    for (const int i : v1) {
        ASSERT_EQ(i, 5);
    }
    v1.clear();
    v1 = std::move(v2);
    ASSERT_TRUE(v2.empty());
    ASSERT_EQ(v2.capacity(), 0);
    for (const int i : v1) {
        ASSERT_EQ(i, 5);
    }

    v1.resize(1);
    ASSERT_EQ(v1, ciel::vector({5}));

    v1.resize(10, 77);
    ASSERT_EQ(v1, ciel::vector({5, 77, 77, 77, 77, 77, 77, 77, 77, 77}));

    v1.pop_back();
    ASSERT_EQ(v1, ciel::vector({5, 77, 77, 77, 77, 77, 77, 77, 77}));

    v1.push_back(123);
    ASSERT_EQ(v1, ciel::vector({5, 77, 77, 77, 77, 77, 77, 77, 77, 123}));

    v1.shrink_to_fit();
    ASSERT_EQ(v1.size(), 10);
    ASSERT_EQ(v1.capacity(), 10);

    v1.reserve(98);
    ASSERT_EQ(v1.size(), 10);
    ASSERT_EQ(v1.capacity(), 98);

    v1.emplace_back(987);
    ASSERT_EQ(v1, ciel::vector({5, 77, 77, 77, 77, 77, 77, 77, 77, 123, 987}));

    v1.push_back(v1.size());
    ASSERT_EQ(v1, ciel::vector({5, 77, 77, 77, 77, 77, 77, 77, 77, 123, 987, 11}));

    v1.shrink_to_fit();
    ASSERT_EQ(v1.capacity(), 12);
    v1.push_back(v1[0]);    // self assignment
    ASSERT_EQ(v1, ciel::vector({5, 77, 77, 77, 77, 77, 77, 77, 77, 123, 987, 11, 5}));
}

TEST(vector_tests, resize2) {
    ciel::vector v1{654};
    ASSERT_EQ(v1.capacity(), 1);

    v1.push_back(v1[0]);    // self assignment
    ASSERT_EQ(v1, ciel::vector({654, 654}));

    v1.resize(5000, 654);
    ASSERT_EQ(v1.size(), 5000);
    for (const int i: v1) {
        ASSERT_EQ(i, 654);
    }
}

TEST(vector_tests, range_construct) {
    ciel::vector v1{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    for (size_t i = 0; i < v1.size(); ++i) {
        ASSERT_EQ(v1[i], i);
        ASSERT_EQ(v1.at(i), i);
    }

    ciel::vector v2(v1.begin(), v1.end() - 2);
    ASSERT_EQ(v2.size(), 8);
    for (size_t i = 0; i < v2.size(); ++i) {
        ASSERT_EQ(v2[i], i);
    }
    ASSERT_EQ(v2.front(), 0);
    ASSERT_EQ(v2.back(), 7);

    ciel::vector v3(std::move(v1));
    for (size_t i = 0; i < v3.size(); ++i) {
        ASSERT_EQ(v3[i], i);
    }
}

TEST(vector_tests, insert_and_erase) {
    ciel::vector v1{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    v1.erase(v1.begin());
    ASSERT_EQ(v1, ciel::vector({1, 2, 3, 4, 5, 6, 7, 8, 9}));

    v1.erase(v1.begin() + 4, v1.begin() + 7);
    ASSERT_EQ(v1, ciel::vector({1, 2, 3, 4, 8, 9}));

    v1.emplace(v1.begin(), 8);
    ASSERT_EQ(v1, ciel::vector({8, 1, 2, 3, 4, 8, 9}));

    v1.insert(v1.begin() + 1, {4, 3, 2});
    ASSERT_EQ(v1, ciel::vector({8, 4, 3, 2, 1, 2, 3, 4, 8, 9}));

    ciel::vector v2{123, 543, 12};
    v1.insert(v1.end(), v2.begin(), v2.begin());    // empty range
    ASSERT_EQ(v1, ciel::vector({8, 4, 3, 2, 1, 2, 3, 4, 8, 9}));

    v1.insert(v1.end() - 1, v2.begin() + 1, v2.end());
    ASSERT_EQ(v1, ciel::vector({8, 4, 3, 2, 1, 2, 3, 4, 8, 543, 12, 9}));

    v1.insert(v1.begin() + 2, 3, 222);
    ASSERT_EQ(v1, ciel::vector({8, 4, 222, 222, 222, 3, 2, 1, 2, 3, 4, 8, 543, 12, 9}));

    ciel::vector<int> v3;
    v3.insert(v3.begin(), v1.begin(), v1.end());
    ASSERT_EQ(v3, ciel::vector({8, 4, 222, 222, 222, 3, 2, 1, 2, 3, 4, 8, 543, 12, 9}));

    v3.insert(v3.end(), v1.begin(), v1.end());
    ASSERT_EQ(v3, ciel::vector({8, 4, 222, 222, 222, 3, 2, 1, 2, 3, 4, 8, 543, 12, 9, 8, 4, 222, 222, 222, 3, 2, 1, 2,
                                3, 4, 8, 543, 12, 9}));

//  v1.insert(v1.begin(), v1.begin() + 1, v1.begin() + 3);    // not allowed
}

TEST(vector_tests, copy_and_move_behavior) {
    ConstructAndAssignCounter::copy = 0;
    ConstructAndAssignCounter::move = 0;

    ciel::vector<ConstructAndAssignCounter> v1(5);
    ASSERT_EQ(ConstructAndAssignCounter::copy, 0);

    ciel::vector<ConstructAndAssignCounter> v2(6, ConstructAndAssignCounter{});
    ASSERT_EQ(ConstructAndAssignCounter::copy, 6);

    const ciel::vector<ConstructAndAssignCounter> v3 = v1;
    const ciel::vector<ConstructAndAssignCounter> v4 = std::move(v2);
    ASSERT_EQ(ConstructAndAssignCounter::copy, 11);

    const ciel::vector<ConstructAndAssignCounter> v5(v1.begin(), v1.end() - 1);
    ASSERT_EQ(ConstructAndAssignCounter::copy, 15);

    ciel::vector<ConstructAndAssignCounter> v6({{}, {}, {}});
    ASSERT_EQ(ConstructAndAssignCounter::copy, 18);

    v6 = {{}, {}, {}, {}};
    ASSERT_EQ(ConstructAndAssignCounter::copy, 22);

    v6.assign(7, {});
    ASSERT_EQ(ConstructAndAssignCounter::copy, 29);

    v6.assign(v1.begin(), v1.end());
    ASSERT_EQ(ConstructAndAssignCounter::copy, 34);

    v6.assign({{}, {}, {}, {}});
    ASSERT_EQ(ConstructAndAssignCounter::copy, 38);

    ASSERT_EQ(ConstructAndAssignCounter::move, 0);
}

TEST(vector_tests, copy_and_move_behavior2) {
    ConstructAndAssignCounter::copy = 0;
    ConstructAndAssignCounter::move = 0;

    ciel::vector<ConstructAndAssignCounter> v1;

    for (size_t i = 0; i < 100; ++i) {
        v1.emplace_back();
    }
    ASSERT_EQ(ConstructAndAssignCounter::copy, 0);

    for (size_t i = 0; i < 100; ++i) {
        v1.push_back({});
    }
    ASSERT_EQ(ConstructAndAssignCounter::copy, 0);

    ConstructAndAssignCounter tmp;

    for (size_t i = 0; i < 100; ++i) {
        v1.push_back(std::move(tmp));
    }
    ASSERT_EQ(ConstructAndAssignCounter::copy, 0);

    for (size_t i = 0; i < 100; ++i) {
        v1.push_back(tmp);
    }
    ASSERT_EQ(ConstructAndAssignCounter::copy, 100);

    ConstructAndAssignCounter::move = 0;

    v1.shrink_to_fit();
    ASSERT_EQ(ConstructAndAssignCounter::copy, 100);
    ASSERT_EQ(ConstructAndAssignCounter::move, 400);
}

TEST(vector_tests, copy_and_move_behavior3) {
    ConstructAndAssignCounter::copy = 0;

    ciel::vector<ConstructAndAssignCounter> v1(10);
    v1.erase(v1.begin());
    ASSERT_EQ(ConstructAndAssignCounter::copy, 0);

    v1.erase(v1.begin() + 5, v1.begin() + 7);
    ASSERT_EQ(ConstructAndAssignCounter::copy, 0);

    v1.insert(v1.begin(), ConstructAndAssignCounter{});
    ASSERT_EQ(ConstructAndAssignCounter::copy, 0);

    const ConstructAndAssignCounter tmp;
    v1.insert(v1.begin(), tmp);
    ASSERT_EQ(ConstructAndAssignCounter::copy, 1);

    v1.insert(v1.begin(), 3, {});
    ASSERT_EQ(ConstructAndAssignCounter::copy, 4);

    v1.insert(v1.begin(), {{}, {}});
    ASSERT_EQ(ConstructAndAssignCounter::copy, 6);

    v1.shrink_to_fit();    // capacity turns to 14
    ASSERT_EQ(ConstructAndAssignCounter::copy, 6);

    v1.insert(v1.end() - 2, v1.begin(), v1.begin() + 2);
    ASSERT_EQ(ConstructAndAssignCounter::copy, 8);
}