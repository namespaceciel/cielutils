#include "benchmark_config.h"

#include <EASTL/set.h>
#include <ciel/set.hpp>
#include <set>

// insert
void set_insert_std(benchmark::State& state) {
    for (auto _ : state) {
        set_insert_benchmark<std::set<uint64_t>>();
    }
}

void set_insert_eastl(benchmark::State& state) {
    for (auto _ : state) {
        set_insert_benchmark<eastl::set<uint64_t>>();
    }
}

void set_insert_ciel(benchmark::State& state) {
    for (auto _ : state) {
        set_insert_benchmark<ciel::set<uint64_t>>();
    }
}

// insert sorted data
void set_sorted_insert_std(benchmark::State& state) {
    for (auto _ : state) {
        set_sorted_insert_benchmark<std::set<uint64_t>>();
    }
}

void set_sorted_insert_eastl(benchmark::State& state) {
    for (auto _ : state) {
        set_sorted_insert_benchmark<eastl::set<uint64_t>>();
    }
}

void set_sorted_insert_ciel(benchmark::State& state) {
    for (auto _ : state) {
        set_sorted_insert_benchmark<ciel::set<uint64_t>>();
    }
}

// find
void set_find_std(benchmark::State& state) {
    std::set<uint64_t> s;
    std::random_device rd;
    std::mt19937_64 g(rd());

    for (int i = 0; i < 100000; ++i) {
        s.insert(g());
    }

    for (auto _ : state) {
        set_find_benchmark(s);
    }
}

void set_find_eastl(benchmark::State& state) {
    eastl::set<uint64_t> s;
    std::random_device rd;
    std::mt19937_64 g(rd());

    for (int i = 0; i < 100000; ++i) {
        s.insert(g());
    }

    for (auto _ : state) {
        set_find_benchmark(s);
    }
}

void set_find_ciel(benchmark::State& state) {
    ciel::set<uint64_t> s;
    std::random_device rd;
    std::mt19937_64 g(rd());

    for (int i = 0; i < 100000; ++i) {
        s.insert(g());
    }

    for (auto _ : state) {
        set_find_benchmark(s);
    }
}

// erase (We don't benchmark erasing certain element because this contains finding element and balancing after erasion)
void set_erase_std(benchmark::State& state) {
    std::set<uint64_t> s;
    std::random_device rd;
    std::mt19937_64 g(rd());

    for (int i = 0; i < 10000; ++i) {
        s.insert(g());
    }

    for (auto _ : state) {
        set_erase_benchmark(s);
    }
}

void set_erase_eastl(benchmark::State& state) {
    eastl::set<uint64_t> s;
    std::random_device rd;
    std::mt19937_64 g(rd());

    for (int i = 0; i < 10000; ++i) {
        s.insert(g());
    }

    for (auto _ : state) {
        set_erase_benchmark(s);
    }
}

void set_erase_ciel(benchmark::State& state) {
    ciel::set<uint64_t> s;
    std::random_device rd;
    std::mt19937_64 g(rd());

    for (int i = 0; i < 10000; ++i) {
        s.insert(g());
    }

    for (auto _ : state) {
        set_erase_benchmark(s);
    }
}