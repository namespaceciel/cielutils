#include <gtest/gtest.h>

#include <ciel/forward_list.hpp>

TEST(forward_list_tests, constructors_and_destructors) {
    ciel::forward_list<int> l1;
    ASSERT_TRUE(l1.empty());
    ASSERT_EQ(l1.size(), 0);
    ASSERT_EQ(l1.begin(), l1.end());

    ciel::forward_list l2(10, 666);
    ASSERT_EQ(l2.size(), 10);
    ASSERT_EQ(*l2.begin(), 666);

    ciel::forward_list<int> l3(10);
    ASSERT_EQ(l3.size(), 10);
    ASSERT_EQ(*l3.begin(), 0);

    ciel::forward_list l4({0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
    ASSERT_EQ(l4.size(), 10);
    ASSERT_EQ(*l4.begin(), 0);

    auto b1 = l4.begin();
    for (int i = 0; i < 10; ++i) {
        ASSERT_EQ(*b1++, i);
    }

    const std::initializer_list<int> ilist{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    ciel::forward_list l5(ilist.begin(), ilist.end());
    ASSERT_EQ(l5.size(), 10);
    ASSERT_EQ(*l5.begin(), 0);
    auto b2 = l5.begin();
    for (int i = 0; i < 10; ++i) {
        ASSERT_EQ(*b2++, i);
    }

    ciel::forward_list l6(l5);
    ASSERT_EQ(l6.size(), 10);
    ASSERT_EQ(*l6.begin(), 0);
    auto b3 = l6.begin();
    for (int i = 0; i < 10; ++i) {
        ASSERT_EQ(*b3++, i);
    }

    ciel::forward_list l7(std::move(l6));
    ASSERT_TRUE(l6.empty());
    ASSERT_EQ(l7.size(), 10);
    ASSERT_EQ(*l7.begin(), 0);
    auto b4 = l7.begin();
    for (int i = 0; i < 10; ++i) {
        ASSERT_EQ(*b4++, i);
    }

    l7.clear();
    ASSERT_TRUE(l1.empty());

    l7 = std::move(l5);
    ASSERT_TRUE(l5.empty());
    ASSERT_EQ(l7.size(), 10);
    ASSERT_EQ(*l7.begin(), 0);
    auto b5 = l7.begin();
    for (int i = 0; i < 10; ++i) {
        ASSERT_EQ(*b5++, i);
    }

    l5 = l7;
    ASSERT_EQ(l5.size(), 10);
    ASSERT_EQ(*l5.begin(), 0);
    auto b6 = l5.begin();
    for (int i = 0; i < 10; ++i) {
        ASSERT_EQ(*b6++, i);
    }

    l1 = ilist;
    ASSERT_EQ(l1.size(), 10);
    ASSERT_EQ(*l1.begin(), 0);
    auto b7 = l5.begin();
    for (int i = 0; i < 10; ++i) {
        ASSERT_EQ(*b7++, i);
    }
}

TEST(forward_list_tests, assign) {
    ciel::forward_list l1{1, 2, 3};
    l1.assign(5, 123);
    ASSERT_EQ(l1, ciel::forward_list({123, 123, 123, 123, 123}));

    l1.assign({432, 53, 1, 67});
    ASSERT_EQ(l1, ciel::forward_list({432, 53, 1, 67}));

    ciel::forward_list l2{654, 433, 21, 987, 655};
    l1.assign(l2.begin(), l2.end());
    ASSERT_EQ(l1, ciel::forward_list({654, 433, 21, 987, 655}));
}

TEST(forward_list_tests, insertions) {
    ciel::forward_list<int> l1;
    l1.push_front(3);
    l1.emplace_front(2);
    l1.push_front(1);
    l1.emplace_front(0);
    ASSERT_EQ(l1, ciel::forward_list({0, 1, 2, 3}));

    l1.insert_after(l1.begin().next(), 123);
    l1.insert_after(l1.begin(), 123);
    ASSERT_EQ(l1, ciel::forward_list({0, 123, 1, 123, 2, 3}));

    l1.insert_after(l1.before_begin(), 3, 666);
    ASSERT_EQ(l1, ciel::forward_list({666, 666, 666, 0, 123, 1, 123, 2, 3}));

    l1.insert_after(l1.begin(), {11, 22, 33});
    ASSERT_EQ(l1, ciel::forward_list({666, 11, 22, 33, 666, 666, 0, 123, 1, 123, 2, 3}));

    ciel::forward_list l2{98, 87, 76};
    l1.insert_after(l1.begin(), l2.begin(), l2.end());
    ASSERT_EQ(l1, ciel::forward_list({666, 98, 87, 76, 11, 22, 33, 666, 666, 0, 123, 1, 123, 2, 3}));

    l1.emplace_after(l1.begin().next(), 87654);
    ASSERT_EQ(l1, ciel::forward_list({666, 98, 87654, 87, 76, 11, 22, 33, 666, 666, 0, 123, 1, 123, 2, 3}));

    l1.erase_after(l1.begin());
    ASSERT_EQ(l1, ciel::forward_list({666, 87654, 87, 76, 11, 22, 33, 666, 666, 0, 123, 1, 123, 2, 3}));

    l1.erase_after(l1.begin(), l1.end());
    ASSERT_EQ(l1, ciel::forward_list({666}));

    l1.resize(5);
    ASSERT_EQ(l1, ciel::forward_list({666, 0, 0, 0, 0}));

    l1.resize(3);
    ASSERT_EQ(l1, ciel::forward_list({666, 0, 0}));

    l1.resize(6, 123);
    ASSERT_EQ(l1, ciel::forward_list({666, 0, 0, 123, 123, 123}));

    l1.pop_front();
    l1.pop_front();
    ASSERT_EQ(l1, ciel::forward_list({0, 123, 123, 123}));
}

TEST(forward_list_tests, swap) {
    ciel::forward_list l1{4, 3, 2, 1};
    ciel::forward_list l2{6, 7, 8, 9, 6, 4, 3};

    std::swap(l1, l2);
    ASSERT_EQ(l1, ciel::forward_list({6, 7, 8, 9, 6, 4, 3}));
    ASSERT_EQ(l2, ciel::forward_list({4, 3, 2, 1}));
}