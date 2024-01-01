#include <gtest/gtest.h>

#include <ciel/memory.hpp>

namespace {

struct Task {
public:
    static Task* create_worker() {
        ciel::unique_ptr<Task> tmp{new Task{}};
        Task* res = tmp.get();
        res->accept(std::move(tmp));
        return res;
    }

    void work_and_die() {
//		std::cout << "Working...\n";
        ptr_.reset();
    }

//	~Task() { std::cout << "Dying...\n"; }

private:
    ciel::unique_ptr<Task> ptr_;    // ptr, this
    void accept(ciel::unique_ptr<Task>&& tmp) noexcept {
        ptr_ = std::move(tmp);
    }
};

struct A {};
struct B : A {};

struct EmptyDeleter {
    void operator()(...) const noexcept {}
};

void IntFunctionDeleter(const int* ptr) noexcept {
    delete ptr;
}

struct StructOperatorDeleter {
    void operator()(const int* ptr) const noexcept {
        delete ptr;
    }
};

struct Constructor_And_Destructor_Counter {
    inline static size_t ctor = 0;
    inline static size_t dtor = 0;
    Constructor_And_Destructor_Counter() noexcept {
        ++ctor;
    }
    ~Constructor_And_Destructor_Counter() {
        ++dtor;
    }
};

struct Loop_ASSERT_EQ2;
struct Loop_ASSERT_EQ1 {
    ciel::shared_ptr<Loop_ASSERT_EQ2> p;
    inline static size_t ctor = 0;
    inline static size_t dtor = 0;
    Loop_ASSERT_EQ1() noexcept {
        ++ctor;
    }
    ~Loop_ASSERT_EQ1() {
        ++dtor;
    }
};
struct Loop_ASSERT_EQ2 {
    ciel::weak_ptr<Loop_ASSERT_EQ1> p;
    inline static size_t ctor = 0;
    inline static size_t dtor = 0;
    Loop_ASSERT_EQ2() noexcept {
        ++ctor;
    }
    ~Loop_ASSERT_EQ2() {
        ++dtor;
    }
};

class Derived_From_Enable_Shared_From_This : public ciel::enable_shared_from_this<Derived_From_Enable_Shared_From_This> {
public:
    // enable_shared_from_this allows us construct a shared_ptr in struct pointing to itself,
    // and be used at thread callback

    [[nodiscard]] auto get_ptr() noexcept -> ciel::shared_ptr<Derived_From_Enable_Shared_From_This> {
        return shared_from_this();
    }

    [[nodiscard]] static auto create() -> ciel::shared_ptr<Derived_From_Enable_Shared_From_This> {
        // constructor is private, can't use make_shared
        return ciel::shared_ptr<Derived_From_Enable_Shared_From_This>(new Derived_From_Enable_Shared_From_This());
    }

private:
    Derived_From_Enable_Shared_From_This() noexcept = default;
};

}   // namespace

TEST(memory_tests, unique_ptr) {
    Task* t = Task::create_worker();
    t->work_and_die();

    const ciel::unique_ptr<int> ptr1;
    const ciel::unique_ptr<int> ptr2(nullptr);
    int* id{};
    const ciel::unique_ptr<int> ptr3(id);
    ASSERT_EQ(ptr1, nullptr);
    ASSERT_EQ(ptr2, nullptr);
    ASSERT_EQ(ptr3, nullptr);

    ciel::unique_ptr<int> ptr4(new int{1});
    ASSERT_EQ(*ptr4, 1);

    const ciel::unique_ptr ptr5(std::move(ptr4));
    ASSERT_EQ(*ptr5, 1);
    ASSERT_EQ(ptr4, nullptr);

    ciel::unique_ptr<B, EmptyDeleter> ptr6;
    const ciel::unique_ptr<A, EmptyDeleter> ptr7(std::move(ptr6));

    const ciel::unique_ptr<int> ptr8 = ciel::make_unique<int>(0);
    const ciel::unique_ptr<A> ptr9 = ciel::make_unique<A>();

    ciel::unique_ptr<int[]> ptr10;
    ciel::unique_ptr ptr11(std::move(ptr10));
    ciel::unique_ptr<int[]> ptr12(new int[]{0, 1, 2, 3, 4});
    for (size_t i = 0; i < 5; ++i) {
        ASSERT_EQ(ptr12[i], i);
    }
    std::swap(ptr11, ptr12);
    for (size_t i = 0; i < 5; ++i) {
        ASSERT_EQ(ptr11[i], i);
    }
    ptr10 = ciel::make_unique<int[]>(5);
    for (size_t i = 0; i < 5; ++i) {
        ASSERT_EQ(ptr10[i], 0);
    }

    auto* arr = new int[]{0, 1, 2, 3, 4};
    const ciel::unique_ptr<int[]> ptr13(arr);

    ciel::unique_ptr<ciel::unique_ptr<int>> ptr14(new ciel::unique_ptr<int>(new int(5)));
    const ciel::unique_ptr ptr15(std::move(ptr14));

    const ciel::unique_ptr<int, void (*)(const int*)> ptr16(new int{5}, IntFunctionDeleter);
    const ciel::unique_ptr<int, StructOperatorDeleter> ptr17(new int{5}, StructOperatorDeleter{});
    const ciel::unique_ptr<int, std::function<void(int*)>> ptr18(new int{5}, [](const int* ptr) { delete ptr; });
    const ciel::unique_ptr<int[], std::function<void(int*)>> ptr19(new int[]{1,5}, [](const int* ptr) { delete[] ptr; });
}

TEST(memory_tests, shared_ptr) {
    using CADC = Constructor_And_Destructor_Counter;
    {
        const ciel::shared_ptr<CADC> ptr;
    }
    ASSERT_EQ(CADC::ctor, 0);
    ASSERT_EQ(CADC::dtor, 0);
    {
        const ciel::shared_ptr<CADC> ptr(new CADC);
    }
    ASSERT_EQ(CADC::ctor, 1);
    ASSERT_EQ(CADC::dtor, 1);
    {
        const ciel::shared_ptr<CADC> ptr(new CADC);
        const ciel::shared_ptr ptr2(ptr);
    }
    ASSERT_EQ(CADC::ctor, 2);
    ASSERT_EQ(CADC::dtor, 2);
    {
        const ciel::shared_ptr<CADC[]> ptr(new CADC[3]);
        const ciel::shared_ptr ptr2(ptr);
    }
    ASSERT_EQ(CADC::ctor, 5);
    ASSERT_EQ(CADC::dtor, 5);

    {
        auto* lc1 = new Loop_ASSERT_EQ1;
        auto* lc2 = new Loop_ASSERT_EQ2;

        const ciel::shared_ptr<Loop_ASSERT_EQ1> ptr1(lc1);
        const ciel::shared_ptr<Loop_ASSERT_EQ2> ptr2(lc2);
        ptr1->p = ptr2;
        ptr2->p = ptr1;
    }
    ASSERT_EQ(Loop_ASSERT_EQ1::ctor, 1);
    ASSERT_EQ(Loop_ASSERT_EQ1::dtor, 1);
    ASSERT_EQ(Loop_ASSERT_EQ2::ctor, 1);
    ASSERT_EQ(Loop_ASSERT_EQ2::dtor, 1);
}

TEST(memory_tests, enable_shared_from_this) {
    const ciel::shared_ptr<Derived_From_Enable_Shared_From_This> p1 = Derived_From_Enable_Shared_From_This::create();
    const ciel::shared_ptr<Derived_From_Enable_Shared_From_This> p2 = p1->get_ptr();
    ASSERT_EQ(p1.use_count(), 2);
    ASSERT_EQ(p2.use_count(), 2);
}