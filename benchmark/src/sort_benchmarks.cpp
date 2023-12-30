#include "benchmark_config.h"

#include <EASTL/sort.h>
#include <algorithm>
#include <ciel/algorithm.hpp>

// sort
static void sort_std(benchmark::State& state) {
    for (auto _ : state) {
        sort_benchmark()(std::sort);
    }
}

static void sort_eastl(benchmark::State& state) {
    for (auto _ : state) {
        sort_benchmark()(eastl::sort);
    }
}

static void sort_ciel(benchmark::State& state) {
    for (auto _ : state) {
        sort_benchmark()(ciel::sort);
    }
}

// sorted_arr_sort
static void sorted_arr_sort_std(benchmark::State& state) {
    for (auto _ : state) {
        sorted_arr_sort_benchmark()(std::sort);
    }
}

static void sorted_arr_sort_eastl(benchmark::State& state) {
    for (auto _ : state) {
        sorted_arr_sort_benchmark()(eastl::sort);
    }
}

static void sorted_arr_sort_ciel(benchmark::State& state) {
    for (auto _ : state) {
        sorted_arr_sort_benchmark()(ciel::sort);
    }
}

// stable_sort
static void stable_sort_std(benchmark::State& state) {
    for (auto _ : state) {
        sort_benchmark()(std::stable_sort);
    }
}

static void stable_sort_eastl(benchmark::State& state) {
    for (auto _ : state) {
        sort_benchmark()(eastl::stable_sort);
    }
}

static void stable_sort_ciel(benchmark::State& state) {
    for (auto _ : state) {
        sort_benchmark()(ciel::stable_sort);
    }
}

BENCHMARK(sort_std);
BENCHMARK(sort_eastl);
BENCHMARK(sort_ciel);

BENCHMARK(sorted_arr_sort_std);
BENCHMARK(sorted_arr_sort_eastl);
BENCHMARK(sorted_arr_sort_ciel);

BENCHMARK(stable_sort_std);
BENCHMARK(stable_sort_eastl);
BENCHMARK(stable_sort_ciel);