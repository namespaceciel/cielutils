#include "benchmark_config.h"

#include <EASTL/deque.h>
#include <ciel/deque.hpp>
#include <ciel/split_buffer.hpp>
#include <deque>

// push_back
static void deque_push_back_std(benchmark::State& state) {
    for (auto _ : state) {
        push_back_benchmark<std::deque<int>>();
    }
}

static void deque_push_back_eastl(benchmark::State& state) {
    for (auto _ : state) {
        push_back_benchmark<eastl::deque<int>>();
    }
}

static void deque_push_back_ciel(benchmark::State& state) {
    for (auto _ : state) {
        push_back_benchmark<ciel::deque<int>>();
    }
}

static void split_buffer_push_back_ciel(benchmark::State& state) {
    for (auto _ : state) {
        push_back_benchmark<ciel::split_buffer<int>>();
    }
}

// push_front
static void deque_push_front_std(benchmark::State& state) {
    for (auto _ : state) {
        push_front_benchmark<std::deque<int>>();
    }
}

static void deque_push_front_eastl(benchmark::State& state) {
    for (auto _ : state) {
        push_front_benchmark<eastl::deque<int>>();
    }
}

static void deque_push_front_ciel(benchmark::State& state) {
    for (auto _ : state) {
        push_front_benchmark<ciel::deque<int>>();
    }
}

static void split_buffer_push_front_ciel(benchmark::State& state) {
    for (auto _ : state) {
        push_front_benchmark<ciel::split_buffer<int>>();
    }
}

// insert
static void deque_insert_std(benchmark::State& state) {
    for (auto _ : state) {
        insert_benchmark<std::deque<int>>();
    }
}

static void deque_insert_eastl(benchmark::State& state) {
    for (auto _ : state) {
        insert_benchmark<eastl::deque<int>>();
    }
}

static void deque_insert_ciel(benchmark::State& state) {
    for (auto _ : state) {
        insert_benchmark<ciel::deque<int>>();
    }
}

static void split_buffer_insert_ciel(benchmark::State& state) {
    for (auto _ : state) {
        insert_benchmark<ciel::split_buffer<int>>();
    }
}

// erase
static void deque_erase_std(benchmark::State& state) {
    for (auto _ : state) {
        erase_benchmark<std::deque<int>>();
    }
}

static void deque_erase_eastl(benchmark::State& state) {
    for (auto _ : state) {
        erase_benchmark<eastl::deque<int>>();
    }
}

static void deque_erase_ciel(benchmark::State& state) {
    for (auto _ : state) {
        erase_benchmark<ciel::deque<int>>();
    }
}

static void split_buffer_erase_ciel(benchmark::State& state) {
    for (auto _ : state) {
        erase_benchmark<ciel::split_buffer<int>>();
    }
}

BENCHMARK(deque_push_back_std);
BENCHMARK(deque_push_back_eastl);
BENCHMARK(deque_push_back_ciel);
BENCHMARK(split_buffer_push_back_ciel);

BENCHMARK(deque_push_front_std);
BENCHMARK(deque_push_front_eastl);
BENCHMARK(deque_push_front_ciel);
BENCHMARK(split_buffer_push_front_ciel);

BENCHMARK(deque_insert_std);
BENCHMARK(deque_insert_eastl);
BENCHMARK(deque_insert_ciel);
BENCHMARK(split_buffer_insert_ciel);

BENCHMARK(deque_erase_std);
BENCHMARK(deque_erase_eastl);
BENCHMARK(deque_erase_ciel);
BENCHMARK(split_buffer_erase_ciel);