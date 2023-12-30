#include "benchmark_config.h"

#include <EASTL/vector.h>
#include <ciel/vector.hpp>
#include <vector>

// push_back
static void vector_push_back_std(benchmark::State& state) {
    for (auto _ : state) {
        push_back_benchmark<std::vector<int>>();
    }
}

static void vector_push_back_eastl(benchmark::State& state) {
    for (auto _ : state) {
        push_back_benchmark<eastl::vector<int>>();
    }
}

static void vector_push_back_ciel(benchmark::State& state) {
    for (auto _ : state) {
        push_back_benchmark<ciel::vector<int>>();
    }
}

// insert
static void vector_insert_std(benchmark::State& state) {
    for (auto _ : state) {
        insert_benchmark<std::vector<int>>();
    }
}

static void vector_insert_eastl(benchmark::State& state) {
    for (auto _ : state) {
        insert_benchmark<eastl::vector<int>>();
    }
}

static void vector_insert_ciel(benchmark::State& state) {
    for (auto _ : state) {
        insert_benchmark<ciel::vector<int>>();
    }
}

// erase
static void vector_erase_std(benchmark::State& state) {
    for (auto _ : state) {
        erase_benchmark<std::vector<int>>();
    }
}

static void vector_erase_eastl(benchmark::State& state) {
    for (auto _ : state) {
        erase_benchmark<eastl::vector<int>>();
    }
}

static void vector_erase_ciel(benchmark::State& state) {
    for (auto _ : state) {
        erase_benchmark<ciel::vector<int>>();
    }
}

BENCHMARK(vector_push_back_std);
BENCHMARK(vector_push_back_eastl);
BENCHMARK(vector_push_back_ciel);

BENCHMARK(vector_insert_std);
BENCHMARK(vector_insert_eastl);
BENCHMARK(vector_insert_ciel);

BENCHMARK(vector_erase_std);
BENCHMARK(vector_erase_eastl);
BENCHMARK(vector_erase_ciel);