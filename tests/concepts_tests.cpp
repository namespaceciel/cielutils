#include <gtest/gtest.h>

#include <ciel/array.hpp>
#include <ciel/concepts.hpp>

namespace {

struct IntLike {
    int v;
};

[[maybe_unused]] void swap(IntLike& lhs, int& rhs) noexcept {
    std::swap(lhs.v, rhs);
}
void swap(int& lhs, IntLike& rhs) noexcept {
    std::swap(lhs, rhs.v);
}

struct HasSwap {
    void swap(HasSwap& /*unused*/) const noexcept {}
};

size_t count = 0;
void swap(HasSwap& /*unused*/, HasSwap& /*unused*/) noexcept { ++count; }

}   // namespace

TEST(concepts_tests, all) {
    ciel::array arr1{0, 1, 2, 3, 4};
    ciel::array arr2{4, 3, 2, 1, 0};

    ciel::ranges::swap(arr1, arr2);
    for (size_t i = 0; i < arr2.size(); ++i) {
        ASSERT_EQ(arr2[i], i);
    }

    int arr3[]{0, 1, 2, 3, 4};
    int arr4[]{4, 3, 2, 1, 0};

    ciel::ranges::swap(arr3, arr4);
    for (size_t i = 0; i < 5; ++i) {
        ASSERT_EQ(arr4[i], i);
    }

    IntLike arr5[]{{4}, {3}, {2}, {1}, {0}};
    ciel::ranges::swap(arr4, arr5);
    for (size_t i = 0; i < 5; ++i) {
        ASSERT_EQ(arr5[i].v, i);
    }

    HasSwap a;
    HasSwap b;
    ciel::ranges::swap(a, b);
    ASSERT_EQ(count, 1);

    using std::swap;
    swap(a, b);
    ASSERT_EQ(count, 2);
}