#include <gtest/gtest.h>

#include <ciel/any.hpp>
#include <ciel/deque.hpp>
#include <ciel/vector.hpp>

TEST(any_tests, all) {
    ciel::any a = 1;
    ASSERT_EQ(ciel::any_cast<int>(a), 1);

    a = 3.14;
    ASSERT_EQ(ciel::any_cast<double>(a), 3.14);

    a = true;
    ASSERT_TRUE(ciel::any_cast<bool>(a));

#ifdef CIEL_HAS_EXCEPTIONS
    a = 1;
    ASSERT_THROW((void)ciel::any_cast<float>(a), std::bad_any_cast);
#endif

    a = 1;
    ASSERT_TRUE(a.has_value());

    a.reset();
    ASSERT_FALSE(a.has_value());

    a = 1;
    int* i = ciel::any_cast<int>(&a);
    ASSERT_EQ(*i, 1);

    // swap with big object
    ASSERT_FALSE(ciel::is_small_object<ciel::deque<int>>::value);

    ciel::any b = ciel::deque<int>{0, 1, 2, 3, 4};
    a.swap(b);

    ASSERT_EQ(ciel::any_cast<ciel::deque<int>>(a), ciel::deque<int>({0, 1, 2, 3, 4}));
    ASSERT_EQ(ciel::any_cast<int>(b), 1);

    // emplace
    a.emplace<ciel::vector<int>>({0, 1, 2, 3, 4});
    ASSERT_EQ(ciel::any_cast<ciel::vector<int>>(a), ciel::vector<int>({0, 1, 2, 3, 4}));
}