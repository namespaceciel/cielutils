#include <gtest/gtest.h>

#include <ciel/vector.hpp>
#include <random>

// We use random number generator to throw some exceptions, use valgrind to test if there are any memory leaks.

namespace {

std::random_device rd;
std::mt19937_64 g(rd());
bool can_throw;     // set this to false to renew state_holder

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
};

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

}   // namespace

TEST(exception_safety_tests, vector_strong) {
    // These vector functins provide strong exception safety when T is nothrow move constructible:
    // emplace_back, push_back, emplace at end(), reserve, shrink_to_fit
    // When these functions throw, they have no effects, so we test if v changes its state in catch block

    ciel::vector<NothrowMoveStruct> v;
    ciel::vector<NothrowMoveStruct> state_holder;

#define VECTOR_STRONG_TEST_CASE(X)  \
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

    for (size_t i = 0; i < 2000; ++i) {
        VECTOR_STRONG_TEST_CASE(v.emplace(v.end(), g()));

        VECTOR_STRONG_TEST_CASE(v.shrink_to_fit());

        VECTOR_STRONG_TEST_CASE(v.emplace_back(g()));

        VECTOR_STRONG_TEST_CASE(v.reserve(g() % 4000));
    }

#undef VECTOR_STRONG_TEST_CASE
}

TEST(exception_safety_tests, vector_basic) {
    // Throw lots of exceptions and use valgrind checking for memory leaks

    ciel::vector<NothrowMoveStruct> v;
    can_throw = true;

#define VECTOR_BASIC_TEST_CASE(X) \
    CIEL_TRY {                    \
                                  \
        X;                        \
                                  \
    } CIEL_CATCH (...) {}

    for (size_t i = 0; i < 10000; ++i) {
        VECTOR_BASIC_TEST_CASE(v.emplace_back());

        VECTOR_BASIC_TEST_CASE(v.resize(g() % 200, 5));

        VECTOR_BASIC_TEST_CASE(v.insert(v.begin() + v.size() / 2, 10, 20));

        VECTOR_BASIC_TEST_CASE(v.emplace_back(1));

        VECTOR_BASIC_TEST_CASE(v.erase(v.begin() + v.size() / 4, v.end() - v.size() / 4));

        const std::initializer_list<NothrowMoveStruct> il{{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}};
        VECTOR_BASIC_TEST_CASE(v.insert(v.begin() + v.size() / 2, il.begin(), il.end()));
    }

#undef VECTOR_BASIC_TEST_CASE
}