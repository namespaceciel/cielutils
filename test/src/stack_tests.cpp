#include <gtest/gtest.h>

#include <ciel/stack.hpp>
#include <ciel/vector.hpp>

TEST(stack_tests, all) {
    const ciel::stack<int> s;

    ciel::vector<int> v;
    for (int i = 9; i >= 0; --i) {
        v.emplace_back(i);
    }

    ciel::stack s2(v);
    ciel::stack s3(std::move(v));

    const ciel::stack s4(s2);
    ciel::stack s5(std::move(s3));

    const std::initializer_list<int> il{9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
    ciel::stack s6(il.begin(), il.end());

    ASSERT_TRUE(v.empty());
    ASSERT_TRUE(s3.empty());

    while (!s2.empty()) {
        v.emplace_back(s2.top());
        s2.pop();
    }
    ASSERT_EQ(v, ciel::vector({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));
    v.clear();

    while (!s5.empty()) {
        v.emplace_back(s5.top());
        s5.pop();
    }
    ASSERT_EQ(v, ciel::vector({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));
    v.clear();

    while (!s6.empty()) {
        v.emplace_back(s6.top());
        s6.pop();
    }
    ASSERT_EQ(v, ciel::vector({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));
    v.clear();

    for (int i = 9; i >= 0; --i) {
        s2.push(i);
    }
    for (int i = 0; i < 10; ++i) {
        ASSERT_EQ(s2.top(), i);
        s2.pop();
    }
}