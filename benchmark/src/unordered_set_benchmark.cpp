#include "benchmark_config.h"

#include <EASTL/unordered_set.h>
#include <ciel/unordered_set.hpp>
#include <unordered_set>

// insert
void unordered_set_insert_std(benchmark::State& state) {
    for (auto _ : state) {
        set_insert_benchmark<std::unordered_set<uint64_t>>();
    }
}

void unordered_set_insert_eastl(benchmark::State& state) {
    for (auto _ : state) {
        set_insert_benchmark<eastl::unordered_set<uint64_t>>();
    }
}

void unordered_set_insert_ciel(benchmark::State& state) {
    for (auto _ : state) {
        set_insert_benchmark<ciel::unordered_set<uint64_t>>();
    }
}

// find
void unordered_set_find_std(benchmark::State& state) {
    std::unordered_set<uint64_t> s;
    std::random_device rd;
    std::mt19937_64 g(rd());

    for (int i = 0; i < 100000; ++i) {
        s.insert(g());
    }

    for (auto _ : state) {
        set_find_benchmark(s);
    }
}

void unordered_set_find_eastl(benchmark::State& state) {
    eastl::unordered_set<uint64_t> s;
    std::random_device rd;
    std::mt19937_64 g(rd());

    for (int i = 0; i < 100000; ++i) {
        s.insert(g());
    }

    for (auto _ : state) {
        set_find_benchmark(s);
    }
}

void unordered_set_find_ciel(benchmark::State& state) {
    ciel::unordered_set<uint64_t> s;
    std::random_device rd;
    std::mt19937_64 g(rd());

    for (int i = 0; i < 100000; ++i) {
        s.insert(g());
    }

    for (auto _ : state) {
        set_find_benchmark(s);
    }
}

// erase
void unordered_set_erase_value_std(benchmark::State& state) {
    std::unordered_set<uint64_t> s;
    std::random_device rd;
    std::mt19937_64 g(rd());

    for (int i = 0; i < 10000; ++i) {
        s.insert(g() % 10000);
    }

    for (auto _ : state) {
        set_erase_value_benchmark(s);
    }
}

void unordered_set_erase_value_eastl(benchmark::State& state) {
    eastl::unordered_set<uint64_t> s;
    std::random_device rd;
    std::mt19937_64 g(rd());

    for (int i = 0; i < 10000; ++i) {
        s.insert(g() % 10000);
    }

    for (auto _ : state) {
        set_erase_value_benchmark(s);
    }
}

void unordered_set_erase_value_ciel(benchmark::State& state) {
    ciel::unordered_set<uint64_t> s;
    std::random_device rd;
    std::mt19937_64 g(rd());

    for (int i = 0; i < 10000; ++i) {
        s.insert(g() % 10000);
    }

    for (auto _ : state) {
        set_erase_value_benchmark(s);
    }
}