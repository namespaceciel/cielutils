#include <gtest/gtest.h>

#include <ciel/array.hpp>
#include <ciel/span.hpp>
#include <ciel/vector.hpp>

TEST(span_tests, empty_span) {
    const ciel::span<int, 0> s1;
    ASSERT_TRUE(s1.empty());
    ASSERT_EQ(s1.size(), 0);
    ASSERT_EQ(s1.size_bytes(), 0);

    const ciel::span<int> s2;
    ASSERT_TRUE(s2.empty());
    ASSERT_EQ(s2.size(), 0);
    ASSERT_EQ(s2.size_bytes(), 0);
}

TEST(span_tests, constructors_of_fixed_span) {
    int stack_arr[5]{0, 1, 2, 3, 4};
    int* heap_arr = new int[5]{0, 1, 2, 3, 4};
    ciel::array arr{0, 1, 2, 3, 4};

    const ciel::span<int, 3> s1{stack_arr, 3};
    ASSERT_FALSE(s1.empty());
    ASSERT_EQ(s1.size(), 3);
    for (size_t i = 0; i < s1.size(); ++i) {
        ASSERT_EQ(s1[i], i);
    }

    const ciel::span<int, 3> s2{heap_arr, 3};
    ASSERT_FALSE(s2.empty());
    ASSERT_EQ(s2.size(), 3);
    for (size_t i = 0; i < s2.size(); ++i) {
        ASSERT_EQ(s2[i], i);
    }

    const ciel::span<int, 4> s3{heap_arr, heap_arr + 4};
    ASSERT_FALSE(s3.empty());
    ASSERT_EQ(s3.size(), 4);
    for (size_t i = 0; i < s3.size(); ++i) {
        ASSERT_EQ(s3[i], i);
    }

    const ciel::span<int, 5> s4{stack_arr};
    ASSERT_FALSE(s4.empty());
    ASSERT_EQ(s4.size(), 5);
    for (size_t i = 0; i < s4.size(); ++i) {
        ASSERT_EQ(s4[i], i);
    }

    const ciel::span<int, 5> s5{arr};
    ASSERT_FALSE(s5.empty());
    ASSERT_EQ(s5.size(), 5);
    for (size_t i = 0; i < s5.size(); ++i) {
        ASSERT_EQ(s5[i], i);
    }

    const ciel::span<int, 5> s6{s5};
    ASSERT_FALSE(s6.empty());
    ASSERT_EQ(s6.size(), 5);
    for (size_t i = 0; i < s6.size(); ++i) {
        ASSERT_EQ(s6[i], i);
    }
}

TEST(span_tests, constructors_of_dynamic_span) {
    int stack_arr[5]{0, 1, 2, 3, 4};
    int* heap_arr = new int[5]{0, 1, 2, 3, 4};
    ciel::array arr{0, 1, 2, 3, 4};
    ciel::vector vec{0, 1, 2, 3 ,4};

    const ciel::span<int> s1{stack_arr, 3};
    ASSERT_FALSE(s1.empty());
    ASSERT_EQ(s1.size(), 3);
    for (size_t i = 0; i < s1.size(); ++i) {
        ASSERT_EQ(s1[i], i);
    }

    const ciel::span<int> s2{heap_arr, 3};
    ASSERT_FALSE(s2.empty());
    ASSERT_EQ(s2.size(), 3);
    for (size_t i = 0; i < s2.size(); ++i) {
        ASSERT_EQ(s2[i], i);
    }

    const ciel::span<int> s3{heap_arr, heap_arr + 4};
    ASSERT_FALSE(s3.empty());
    ASSERT_EQ(s3.size(), 4);
    for (size_t i = 0; i < s3.size(); ++i) {
        ASSERT_EQ(s3[i], i);
    }

    const ciel::span<int> s4{stack_arr};
    ASSERT_FALSE(s4.empty());
    ASSERT_EQ(s4.size(), 5);
    for (size_t i = 0; i < s4.size(); ++i) {
        ASSERT_EQ(s4[i], i);
    }

    const ciel::span<int> s5{arr};
    ASSERT_FALSE(s5.empty());
    ASSERT_EQ(s5.size(), 5);
    for (size_t i = 0; i < s5.size(); ++i) {
        ASSERT_EQ(s5[i], i);
    }

    const ciel::span<int> s6{s5};
    ASSERT_FALSE(s6.empty());
    ASSERT_EQ(s6.size(), 5);
    for (size_t i = 0; i < s6.size(); ++i) {
        ASSERT_EQ(s6[i], i);
    }

    const ciel::span<int> s7{vec.data(), vec.size()};
    ASSERT_FALSE(s7.empty());
    ASSERT_EQ(s7.size(), 5);
    for (size_t i = 0; i < s7.size(); ++i) {
        ASSERT_EQ(s7[i], i);
    }

    const ciel::span<int> s8{vec.begin(), vec.end()};
    ASSERT_FALSE(s8.empty());
    ASSERT_EQ(s8.size(), 5);
    for (size_t i = 0; i < s8.size(); ++i) {
        ASSERT_EQ(s8[i], i);
    }
}

TEST(span_tests, constructors_from_spans) {
    ciel::vector vec{0, 1, 2, 3, 4};

    const ciel::span<int> s1{vec.data(), 5};
    const ciel::span<int, 4> s2{s1};
    ASSERT_EQ(s2.size(), 4);

    for (size_t i = 0; i < s2.size(); ++i) {
        ASSERT_EQ(s2[i], i);
    }

    const ciel::span<int> s3{s2};
    ASSERT_EQ(s3.size(), 4);
    for (size_t i = 0; i < s3.size(); ++i) {
        ASSERT_EQ(s3[i], i);
    }
}