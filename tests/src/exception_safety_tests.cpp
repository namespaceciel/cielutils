#include <gtest/gtest.h>

#include <ciel/algorithm.hpp>
#include <ciel/deque.hpp>
#include <ciel/forward_list.hpp>
#include <ciel/list.hpp>
#include <ciel/vector.hpp>
#include <random>

// We use random number generator to throw some exceptions, use valgrind to test if there are any memory leaks.

namespace {

std::random_device rd;
std::mt19937_64 g(rd());
bool can_throw;     // set this false to renew state_holder

void may_throw() {
    if (can_throw && g() % 5 == 0) {
        CIEL_THROW(1);
    }
}

struct NothrowMoveStruct {
    size_t* ptr{nullptr};

    NothrowMoveStruct() noexcept = default;

    NothrowMoveStruct(size_t i) {
        may_throw();
        ptr = new size_t{i};
    }

    NothrowMoveStruct(const NothrowMoveStruct& other) {
        may_throw();
        if (other.ptr) {
            ptr = new size_t{*other.ptr};
        }
    }

    NothrowMoveStruct(NothrowMoveStruct&& other) noexcept {
        ptr = other.ptr;
        other.ptr = nullptr;
    }

    ~NothrowMoveStruct() {
        if (ptr) {
            delete ptr;
        }
    }

    auto operator=(const NothrowMoveStruct& other) -> NothrowMoveStruct& {
        if (this == ciel::addressof(other)) {
            return *this;
        }
        may_throw();
        if (ptr) {
            if (other.ptr) {
                *ptr = *other.ptr;
            } else {
                delete ptr;
                ptr = nullptr;
            }
        } else if (other.ptr) {
            ptr = new size_t{*other.ptr};
        }
        return *this;
    }

    auto operator=(NothrowMoveStruct&& other) noexcept -> NothrowMoveStruct& {
        if (this == ciel::addressof(other)) {
            return *this;
        }
        if (ptr) {
            delete ptr;
        }
        ptr = other.ptr;
        other.ptr = nullptr;
        return *this;
    }

    operator size_t() const noexcept {
        return ptr ? *ptr : 1234;
    }

};  // struct NothrowMoveStruct

auto operator==(const NothrowMoveStruct& lhs, const NothrowMoveStruct& rhs) -> bool {
    if (lhs.ptr) {
        if (rhs.ptr) {
            return *lhs.ptr == *rhs.ptr;
        }
        return false;
    } else if (rhs.ptr) {
        return false;
    }
    return true;
}

const std::initializer_list<NothrowMoveStruct> il{{11}, {12}, {13}, {}, {14}, {15}, {}, {16},
                                                  {}, {17}, {18}, {}, {19}, {}, {20}};

}   // namespace

#define STRONG_TEST_CASE(X)         \
    can_throw = false;              \
    state_holder = v;               \
    CIEL_TRY {                      \
        can_throw = true;           \
                                    \
        X;                          \
                                    \
    } CIEL_CATCH (...) {            \
        ASSERT_EQ(v, state_holder); \
    }

#define BASIC_TEST_CASE(X)          \
    CIEL_TRY {                      \
                                    \
        X;                          \
                                    \
    } CIEL_CATCH (...) {}

TEST(exception_safety_tests, vector_strong) {
    // These vector functions provide strong exception safety:
    // emplace_back, push_back, insert, emplace, reserve, shrink_to_fit, resize
    // When these functions throw, they have no effects, so we test if v changes its state in catch block

    ciel::vector<NothrowMoveStruct> v;
    ciel::vector<NothrowMoveStruct> state_holder;

    for (size_t i = 0; i < 2000; ++i) {
        STRONG_TEST_CASE(v.emplace(v.end(), 1));

        STRONG_TEST_CASE(v.insert(v.begin() + g() % ciel::max<size_t>(v.size(), 1), 10, 20));

        STRONG_TEST_CASE(v.shrink_to_fit());

        STRONG_TEST_CASE(v.insert(v.begin() + g() % ciel::max<size_t>(v.size(), 1), il));

        STRONG_TEST_CASE(v.resize(g() % (v.size() * 2 + 1), 5));

        STRONG_TEST_CASE(v.emplace_back(2));

        STRONG_TEST_CASE(v.reserve(g() % 4000));
    }
}

TEST(exception_safety_tests, vector_basic) {
    // Throw lots of exceptions and use valgrind checking for memory leaks

    ciel::vector<NothrowMoveStruct> v;
    can_throw = true;

    for (size_t i = 0; i < 10000; ++i) {
        // Use random numbers to insert or erase at any position in v: v.begin() + g() % ciel::max<size_t>(v.size(), 1)

        BASIC_TEST_CASE(v.emplace_back());

        BASIC_TEST_CASE(v.assign(il));

        BASIC_TEST_CASE(v.resize(g() % (v.size() * 2 + 1), 5));

        BASIC_TEST_CASE(v.insert(v.begin() + g() % ciel::max<size_t>(v.size(), 1), 10, 20));

        BASIC_TEST_CASE(v.assign(10, 20));

        BASIC_TEST_CASE(v.emplace_back(1));

        BASIC_TEST_CASE(v.erase(v.begin() + g() % ciel::max<size_t>(v.size(), 1),
                                       v.begin() + g() % ciel::max<size_t>(v.size(), 1)));

        BASIC_TEST_CASE(v.insert(v.begin() + g() % ciel::max<size_t>(v.size(), 1), il));
    }
}

TEST(exception_safety_tests, forward_list_strong) {
    // These forward_list functions provide strong exception safety:
    // insert_after, emplace_after, push_front, emplace_front

    ciel::forward_list<NothrowMoveStruct> v;
    ciel::forward_list<NothrowMoveStruct> state_holder;

    for (size_t i = 0; i < 1000; ++i) {
        STRONG_TEST_CASE(v.emplace_front(1));

        STRONG_TEST_CASE(v.insert_after(v.before_begin(), 10, 20));

        STRONG_TEST_CASE(v.insert_after(v.before_begin(), il));

        STRONG_TEST_CASE(v.emplace_after(v.before_begin(), 2));
    }
}

TEST(exception_safety_tests, forward_list_basic) {
    // Throw lots of exceptions and use valgrind checking for memory leaks

    ciel::forward_list<NothrowMoveStruct> v;
    can_throw = true;

    for (size_t i = 0; i < 10000; ++i) {
        BASIC_TEST_CASE(v.assign(10, 20));

        BASIC_TEST_CASE(v.assign(il));

        BASIC_TEST_CASE(v.resize(g() % (v.size() * 2 + 1), 5));
    }
}

TEST(exception_safety_tests, list_strong) {
    // These list functions provide strong exception safety:
    // insert, emplace, push_back, emplace_back, push_front, emplace_front

    ciel::list<NothrowMoveStruct> v;
    ciel::list<NothrowMoveStruct> state_holder;

    for (size_t i = 0; i < 1000; ++i) {
        STRONG_TEST_CASE(v.emplace_front(1));

        STRONG_TEST_CASE(v.emplace_back(2));

        STRONG_TEST_CASE(v.insert(v.end(), 10, 20));

        STRONG_TEST_CASE(v.insert(v.begin(), il));

        STRONG_TEST_CASE(v.emplace(v.end(), 3));
    }
}

TEST(exception_safety_tests, list_basic) {
    // Throw lots of exceptions and use valgrind checking for memory leaks

    ciel::list<NothrowMoveStruct> v;
    can_throw = true;

    for (size_t i = 0; i < 10000; ++i) {

        BASIC_TEST_CASE(v.assign(10, 20));

        BASIC_TEST_CASE(v.assign(il));

        BASIC_TEST_CASE(v.resize(g() % (v.size() * 2 + 1), 5));
    }
}

TEST(exception_safety_tests, deque_strong) {
    // These deque functions provide strong exception safety:
    // emplace_front/back, push_front/back, insert, emplace, resize

    ciel::deque<NothrowMoveStruct> v;
    ciel::deque<NothrowMoveStruct> state_holder;

    for (size_t i = 0; i < 2000; ++i) {

        STRONG_TEST_CASE(v.emplace(v.end(), 1));

        STRONG_TEST_CASE(v.insert(v.begin() + g() % ciel::max<size_t>(v.size(), 1), 10, 66));

        STRONG_TEST_CASE(v.emplace_back(2));

        STRONG_TEST_CASE(v.resize(g() % (v.size() * 2 + 1), 5));

        STRONG_TEST_CASE(v.emplace(v.begin(), 3));

        STRONG_TEST_CASE(v.insert(v.begin() + g() % ciel::max<size_t>(v.size(), 1), il));

        STRONG_TEST_CASE(v.emplace_front(4));
    }
}

TEST(exception_safety_tests, deque_basic) {
    // Throw lots of exceptions and use valgrind checking for memory leaks

    ciel::deque<NothrowMoveStruct> v;
    can_throw = true;

    for (size_t i = 0; i < 10000; ++i) {
        // Use random numbers to insert or erase at any position in v: v.begin() + g() % ciel::max<size_t>(v.size(), 1)

        BASIC_TEST_CASE(v.emplace_back(1));

        BASIC_TEST_CASE(v.assign(il));

        BASIC_TEST_CASE(v.resize(g() % (v.size() * 2 + 1), 5));

        BASIC_TEST_CASE(v.insert(v.begin() + g() % ciel::max<size_t>(v.size(), 1), 10, 66));

        BASIC_TEST_CASE(v.assign(10, 20));

        BASIC_TEST_CASE(v.emplace_front(2));

        BASIC_TEST_CASE(v.erase(v.begin() + g() % ciel::max<size_t>(v.size(), 1),
                                v.begin() + g() % ciel::max<size_t>(v.size(), 1)));

        BASIC_TEST_CASE(v.insert(v.begin() + g() % ciel::max<size_t>(v.size(), 1), il));
    }
}

#undef STRONG_TEST_CASE
#undef BASIC_TEST_CASE