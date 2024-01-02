#include "benchmark_config.h"

#include <EASTL/sort.h>
#include <algorithm>
#include <ciel/algorithm.hpp>

// sort
void sort_std(benchmark::State& state) {
    for (auto _ : state) {
        sort_benchmark()(std::sort);
    }
}

void sort_eastl(benchmark::State& state) {
    for (auto _ : state) {
        sort_benchmark()(eastl::sort);
    }
}

void sort_ciel(benchmark::State& state) {
    for (auto _ : state) {
        sort_benchmark()(ciel::sort);
    }
}

// sorted_arr_sort
void sorted_arr_sort_std(benchmark::State& state) {
    for (auto _ : state) {
        sorted_arr_sort_benchmark()(std::sort);
    }
}

void sorted_arr_sort_eastl(benchmark::State& state) {
    for (auto _ : state) {
        sorted_arr_sort_benchmark()(eastl::sort);
    }
}

void sorted_arr_sort_ciel(benchmark::State& state) {
    for (auto _ : state) {
        sorted_arr_sort_benchmark()(ciel::sort);
    }
}

// stable_sort
void stable_sort_std(benchmark::State& state) {
    for (auto _ : state) {
        sort_benchmark()(std::stable_sort);
    }
}

void stable_sort_eastl(benchmark::State& state) {
    for (auto _ : state) {
        sort_benchmark()(eastl::stable_sort);
    }
}

void stable_sort_ciel(benchmark::State& state) {
    for (auto _ : state) {
        sort_benchmark()(ciel::stable_sort);
    }
}

// sorted_arr_stable_sort
void sorted_arr_stable_sort_std(benchmark::State& state) {
    for (auto _ : state) {
        sorted_arr_sort_benchmark()(std::stable_sort);
    }
}

void sorted_arr_stable_sort_eastl(benchmark::State& state) {
    for (auto _ : state) {
        sorted_arr_sort_benchmark()(eastl::stable_sort);
    }
}

void sorted_arr_stable_sort_ciel(benchmark::State& state) {
    for (auto _ : state) {
        sorted_arr_sort_benchmark()(ciel::stable_sort);
    }
}