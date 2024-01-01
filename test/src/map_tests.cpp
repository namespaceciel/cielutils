#include <gtest/gtest.h>

#include <ciel/map.hpp>
#include <ciel/vector.hpp>

TEST(map_tests, constructors_and_assignments) {
    const ciel::map<int, int> s0;

    const ciel::map<int, int> s1{{0, 6}, {1, 7}, {2, 3}, {3, 5}, {4, 1}, {5, 0}};

    const ciel::map<int, int> s2{{5, 5}, {3, 3}, {1, 7}, {4, 2}, {0, 1}, {2, 0}};

    const ciel::vector<ciel::pair<int, int>> v1{{5, 5}, {3, 3}, {1, 7}, {4, 2}, {0, 1}, {2, 0}};

    const ciel::map s3(v1.begin(), v1.end());

    const ciel::map<int, int>
        s4{{0, 2}, {3, 2}, {1, 4}, {1, 2}, {4, 0}, {5, 0}, {3, 1}, {0, 5}, {0, 5}, {3, 0}, {2, 1}, {3, 4}};

    const ciel::vector<ciel::pair<int, int>>
        v2{{5, 1}, {3, 1}, {4, 0}, {5, 1}, {3, 0}, {4, 5}, {0, 0}, {2, 3}, {1, 4}, {4, 0}, {2, 3}};

    ciel::map s5(v2.begin(), v2.end());

    ciel::map s6(s5);

    ASSERT_TRUE(s0.empty());
    ASSERT_EQ(s0.size(), 0);

    ASSERT_EQ(s1.size(), 6);
    ASSERT_EQ(s2.size(), 6);
    ASSERT_EQ(s3.size(), 6);
    ASSERT_EQ(s4.size(), 6);
    ASSERT_EQ(s5.size(), 6);
    ASSERT_EQ(s6.size(), 6);

    int i = 0;
    for (auto it = s1.begin(); it != s1.end(); ++it) {
        ASSERT_EQ(it->first, i++);
    }

    i = 5;
    for (auto it = s1.rbegin(); it != s1.rend(); ++it) {
        ASSERT_EQ(it->first, i--);
    }

    ASSERT_NE(s1, s2);
    ASSERT_NE(s1, s3);
    ASSERT_NE(s1, s4);
    ASSERT_NE(s1, s5);
    ASSERT_NE(s1, s6);

    ciel::map s7(std::move(s6));
    ASSERT_TRUE(s6.empty());
    ASSERT_NE(s1, s7);

    s5 = s7;
    ASSERT_EQ(s5, s7);

    s5 = s6;
    ASSERT_EQ(s5, s6);
    ASSERT_TRUE(s5.empty());

    s5 = std::move(s7);
    ASSERT_NE(s5, s6);
    ASSERT_TRUE(s7.empty());

    s5 = {{2, 1}, {5, 3}, {0, 4}, {0, 1}, {2, 3}, {4, 5}, {0, 2}, {1, 3}, {5, 4}, {0, 1}, {2, 3}, {4, 5}, {2, 1},
          {3, 4}, {1, 2}, {5, 0}};
    ASSERT_NE(s1, s5);

    s7.swap(s5);
    ASSERT_TRUE(s5.empty());
    ASSERT_NE(s1, s7);
}

TEST(map_tests, insert_and_delete) {
    ciel::map<int, int> s1;

    s1.insert({{0, 2}, {1, 3}, {4, 0}, {3, 4}, {5, 0}, {2, 1}, {3, 5}, {4, 0}, {6, 2}, {3, 5}, {2, 1}, {0, 2}, {1, 3},
               {3, 0}, {6, 4}, {1, 2}});
    s1.emplace_hint(s1.end(), 7, 8);
    s1.emplace_hint(s1.begin(), 8, 0);
    s1.emplace(1, 2);
    s1.emplace(10, 10);
    s1.emplace_hint(s1.end(), 9, 3);
    s1.insert_or_assign(6, 3);
    s1.insert_or_assign(11, 2);
    s1[3] = 6;
    s1[12] = 1;

    const ciel::map<int, int> tmp1
        {{0, 2}, {1, 3}, {2, 1}, {3, 6}, {4, 0}, {5, 0}, {6, 3}, {7, 8}, {8, 0}, {9, 3}, {10, 10}, {11, 2}, {12, 1}};
    ASSERT_EQ(s1, tmp1);

    s1.erase(5);
    s1.erase(0);
    s1.erase(8);

    const ciel::map<int, int> tmp2{{1, 3}, {2, 1}, {3, 6}, {4, 0}, {6, 3}, {7, 8}, {9, 3}, {10, 10}, {11, 2}, {12, 1}};
    ASSERT_EQ(s1, tmp2);

    ciel::vector<ciel::pair<int, int>> v1{{8, 0}, {5, 5}};
    s1.insert(v1.begin(), v1.end());
    s1.try_emplace(13, 1);
    s1.try_emplace(11, 11);

    const ciel::map<int, int> tmp3
        {{1, 3}, {2, 1}, {3, 6}, {4, 0}, {5, 5}, {6, 3}, {7, 8}, {8, 0}, {9, 3}, {10, 10}, {11, 2}, {12, 1}, {13, 1}};
    ASSERT_EQ(s1, tmp3);

    s1.clear();
    ASSERT_TRUE(s1.empty());
}

TEST(map_tests, find) {
    ciel::map<int, int>
        s1 = {{3, 1}, {0, 4}, {5, 7}, {0, 3}, {1, 2}, {4, 0}, {1, 0}, {3, 8}, {4, 6}, {9, 5}, {2, 9}, {4, 1}};

    ASSERT_EQ(s1.find(3)->second, 1);
    ASSERT_TRUE(s1.contains(9));
    ASSERT_EQ(s1.count(0), 1);
    ASSERT_EQ(s1.lower_bound(4)->second, 0);
    ASSERT_EQ(s1.upper_bound(7)->second, 5);
    ASSERT_EQ(s1[5], 7);
    ASSERT_EQ(s1[2], 9);
    ASSERT_EQ(s1.begin()->second, 4);

    const auto er = s1.equal_range(5);
    ASSERT_EQ(ciel::distance(er.first, er.second), 1);
}