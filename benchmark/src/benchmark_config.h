#ifndef CIELUTILS_BENCHMARK_CONFIG_H
#define CIELUTILS_BENCHMARK_CONFIG_H

#include <benchmark/benchmark.h>

#include <algorithm>
#include <random>

// needed by EASTL
inline void* __cdecl operator new[](const size_t size,
                                    const char* name,
                                    const int flags,
                                    const unsigned debugFlags,
                                    const char* file,
                                    const int line) {
    return new uint8_t[size];
}

template<class Container, class value_type = typename Container::value_type>
    requires requires (Container c, value_type value) { c.push_back(value); }
void push_back_benchmark() noexcept {
    Container c;

    for (int i = 0; i < 100000; ++i) {
        c.push_back(i);
    }
}

template<class Container, class value_type = typename Container::value_type>
    requires requires (Container c, value_type value) { c.push_front(value); }
void push_front_benchmark() noexcept {
    Container c;

    for (int i = 0; i < 100000; ++i) {
        c.push_front(i);
    }
}

template<class Container, class value_type = typename Container::value_type, class iterator = typename Container::iterator>
    requires requires (Container c, value_type value, iterator it) { it = c.insert(it, value); }
void insert_benchmark() noexcept {
    Container c;

    iterator it = c.begin();

    for (int i = 0; i < 1000; ++i) {
        it = c.insert(it, i);

        // Try to safely increment the iterator three times.
        if(it == c.end()) {
            it = c.begin();
        }
        if(++it == c.end()) {
            it = c.begin();
        }
        if(++it == c.end()) {
            it = c.begin();
        }
    }
}

template<class Container, class iterator = typename Container::iterator>
    requires requires (Container c, iterator it) { it = c.erase(it); }
void erase_benchmark() noexcept {
    Container c(1000);

    iterator it = c.begin();

    for (int i = 0;  i < 1000; ++i) {
        it = c.erase(it);

        // Try to safely increment the iterator three times.
        if(it == c.end()) {
            it = c.begin();
        }
        if(++it == c.end()) {
            it = c.begin();
        }
        if(++it == c.end()) {
            it = c.begin();
        }
    }
}

// We need to make a functor for sort_benchmark since it needs an generated unsorted container
struct sort_benchmark {
    uint64_t arr[100000];

    sort_benchmark() noexcept {
        std::random_device rd;
        const std::mt19937_64 g(rd());
        std::ranges::generate(arr, g);
    }

    auto operator()(void(*sort)(uint64_t*, uint64_t*)) noexcept -> void {
        sort(std::begin(arr), std::end(arr));
    }
};

struct sorted_arr_sort_benchmark {
    uint64_t arr[100000];

    sorted_arr_sort_benchmark() noexcept {
        std::iota(std::begin(arr), std::end(arr), 0);
    }

    auto operator()(void(*sort)(uint64_t*, uint64_t*)) noexcept -> void {
        sort(std::begin(arr), std::end(arr));
    }
};

template<class Container, class value_type = typename Container::value_type>
    requires requires (Container c, value_type value) { c.insert(value); } && std::is_same_v<value_type, uint64_t>
void set_insert_benchmark() noexcept {
    Container c;
    std::random_device rd;
    std::mt19937_64 g(rd());

    for (int i = 0; i < 100000; ++i) {
        c.insert(g());
    }
}

// set insert sorted data to test balance performance
template<class Container, class value_type = typename Container::value_type>
    requires requires (Container c, value_type value) { c.insert(value); } && std::is_same_v<value_type, uint64_t>
void set_sorted_insert_benchmark() noexcept {
    Container c;

    for (uint64_t i = 0; i < 100000ULL; ++i) {
        c.insert(i);
    }
}

template<class Set, class value_type = typename Set::value_type>
    requires requires (Set s, value_type value) { s.find(value); } && std::is_same_v<value_type, uint64_t>
void set_find_benchmark(Set& s) noexcept {
    std::random_device rd;
    std::mt19937_64 g(rd());

    for (int i = 0; i < 100000; ++i) {
        (void)s.find(g());
    }
}

template<class Set, class iterator = typename Set::iterator>
void set_erase_benchmark(Set s) noexcept {
    iterator it = s.begin();

    for (int i = 0;  i < 1000; ++i) {
        it = s.erase(it);

        // Try to safely increment the iterator three times.
        if(it == s.end()) {
            it = s.begin();
        }
        if(++it == s.end()) {
            it = s.begin();
        }
        if(++it == s.end()) {
            it = s.begin();
        }
    }
}

#endif // CIELUTILS_BENCHMARK_CONFIG_H