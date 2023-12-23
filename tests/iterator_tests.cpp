#include <gtest/gtest.h>

#include <ciel/iterator.hpp>

namespace {

struct Struct {};

struct IterHasFiveMembers {
    using difference_type = int;
    using value_type = long;
    using pointer = long long;
    using reference = char;
    using iterator_category = size_t;
};

struct IterHasFourMembers {
    using difference_type = int;
    using value_type = long;
    using reference = char;
    using iterator_category = size_t;
};

}   // namespace

TEST(iterator_tests, iterator_traits) {
    // To be compatible with standard library, contiguous_iterator's iterator_category is random_access_iterator_tag

    static_assert(ciel::is_same_v<ciel::iterator_traits<int*>::difference_type, ptrdiff_t>);
    static_assert(ciel::is_same_v<ciel::iterator_traits<int*>::value_type, int>);
    static_assert(ciel::is_same_v<ciel::iterator_traits<int*>::pointer, int*>);
    static_assert(ciel::is_same_v<ciel::iterator_traits<int*>::reference, int&>);
    static_assert(ciel::is_same_v<ciel::iterator_traits<int*>::iterator_category, ciel::random_access_iterator_tag>);

    static_assert(ciel::is_same_v<ciel::iterator_traits<const int*>::difference_type, ptrdiff_t>);
    static_assert(ciel::is_same_v<ciel::iterator_traits<const int*>::value_type, int>);
    static_assert(ciel::is_same_v<ciel::iterator_traits<const int*>::pointer, const int*>);
    static_assert(ciel::is_same_v<ciel::iterator_traits<const int*>::reference, const int&>);
    static_assert(ciel::is_same_v<ciel::iterator_traits<const int*>::iterator_category,
                                  ciel::random_access_iterator_tag>);

    static_assert(ciel::is_same_v<ciel::iterator_traits<const Struct*>::difference_type, ptrdiff_t>);
    static_assert(ciel::is_same_v<ciel::iterator_traits<const Struct*>::value_type, Struct>);
    static_assert(ciel::is_same_v<ciel::iterator_traits<const Struct*>::pointer, const Struct*>);
    static_assert(ciel::is_same_v<ciel::iterator_traits<const Struct*>::reference, const Struct&>);
    static_assert(ciel::is_same_v<ciel::iterator_traits<const Struct*>::iterator_category,
                                  ciel::random_access_iterator_tag>);

    static_assert(ciel::is_same_v<ciel::iterator_traits<IterHasFiveMembers>::difference_type, int>);
    static_assert(ciel::is_same_v<ciel::iterator_traits<IterHasFiveMembers>::value_type, long>);
    static_assert(ciel::is_same_v<ciel::iterator_traits<IterHasFiveMembers>::pointer, long long>);
    static_assert(ciel::is_same_v<ciel::iterator_traits<IterHasFiveMembers>::reference, char>);
    static_assert(ciel::is_same_v<ciel::iterator_traits<IterHasFiveMembers>::iterator_category, size_t>);

    static_assert(ciel::is_same_v<ciel::iterator_traits<IterHasFourMembers>::difference_type, int>);
    static_assert(ciel::is_same_v<ciel::iterator_traits<IterHasFourMembers>::value_type, long>);
    static_assert(ciel::is_same_v<ciel::iterator_traits<IterHasFourMembers>::pointer, void>);
    static_assert(ciel::is_same_v<ciel::iterator_traits<IterHasFourMembers>::reference, char>);
    static_assert(ciel::is_same_v<ciel::iterator_traits<IterHasFourMembers>::iterator_category, size_t>);

    static_assert(ciel::is_same_v<ciel::iterator_traits<ciel::wrap_iter<int*>>::difference_type, ptrdiff_t>);
    static_assert(ciel::is_same_v<ciel::iterator_traits<ciel::wrap_iter<int*>>::value_type, int>);
    static_assert(ciel::is_same_v<ciel::iterator_traits<ciel::wrap_iter<int*>>::pointer, int*>);
    static_assert(ciel::is_same_v<ciel::iterator_traits<ciel::wrap_iter<int*>>::reference, int&>);
    static_assert(ciel::is_same_v<ciel::iterator_traits<ciel::wrap_iter<int*>>::iterator_category,
                                  ciel::random_access_iterator_tag>);
}

TEST(iterator_tests, reverse_iterator) {
    int arr1[10] = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};

    ciel::reverse_iterator<int*> ri1(arr1 + 10);

    for (size_t i = 0; i < 10; ++i) {
        ASSERT_EQ(ri1[i], i);
    }

    for (size_t i = 0; i < 10; ++i) {
        ASSERT_EQ(*ri1, i);
        ++ri1;
    }
}