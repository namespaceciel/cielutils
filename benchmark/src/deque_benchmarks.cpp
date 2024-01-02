#include "benchmark_config.h"

#include <EASTL/deque.h>
#include <ciel/circular_buffer.hpp>
#include <ciel/deque.hpp>
#include <ciel/split_buffer.hpp>
#include <deque>

// push_back
void deque_push_back_std(benchmark::State& state) {
    for (auto _ : state) {
        push_back_benchmark<std::deque<int>>();
    }
}

void deque_push_back_eastl(benchmark::State& state) {
    for (auto _ : state) {
        push_back_benchmark<eastl::deque<int>>();
    }
}

void deque_push_back_ciel(benchmark::State& state) {
    for (auto _ : state) {
        push_back_benchmark<ciel::deque<int>>();
    }
}

void split_buffer_push_back_ciel(benchmark::State& state) {
    for (auto _ : state) {
        push_back_benchmark<ciel::split_buffer<int>>();
    }
}

void circular_buffer_push_back_ciel(benchmark::State& state) {
    for (auto _ : state) {
        push_back_benchmark<ciel::circular_buffer<int>>();
    }
}

// push_front
void deque_push_front_std(benchmark::State& state) {
    for (auto _ : state) {
        push_front_benchmark<std::deque<int>>();
    }
}

void deque_push_front_eastl(benchmark::State& state) {
    for (auto _ : state) {
        push_front_benchmark<eastl::deque<int>>();
    }
}

void deque_push_front_ciel(benchmark::State& state) {
    for (auto _ : state) {
        push_front_benchmark<ciel::deque<int>>();
    }
}

void split_buffer_push_front_ciel(benchmark::State& state) {
    for (auto _ : state) {
        push_front_benchmark<ciel::split_buffer<int>>();
    }
}

void circular_buffer_push_front_ciel(benchmark::State& state) {
    for (auto _ : state) {
        push_front_benchmark<ciel::circular_buffer<int>>();
    }
}

// insert
void deque_insert_std(benchmark::State& state) {
    for (auto _ : state) {
        insert_benchmark<std::deque<int>>();
    }
}

void deque_insert_eastl(benchmark::State& state) {
    for (auto _ : state) {
        insert_benchmark<eastl::deque<int>>();
    }
}

void deque_insert_ciel(benchmark::State& state) {
    for (auto _ : state) {
        insert_benchmark<ciel::deque<int>>();
    }
}

void split_buffer_insert_ciel(benchmark::State& state) {
    for (auto _ : state) {
        insert_benchmark<ciel::split_buffer<int>>();
    }
}

void circular_buffer_insert_ciel(benchmark::State& state) {
    for (auto _ : state) {
        insert_benchmark<ciel::circular_buffer<int>>();
    }
}

// erase
void deque_erase_std(benchmark::State& state) {
    for (auto _ : state) {
        erase_benchmark<std::deque<int>>();
    }
}

void deque_erase_eastl(benchmark::State& state) {
    for (auto _ : state) {
        erase_benchmark<eastl::deque<int>>();
    }
}

void deque_erase_ciel(benchmark::State& state) {
    for (auto _ : state) {
        erase_benchmark<ciel::deque<int>>();
    }
}

void split_buffer_erase_ciel(benchmark::State& state) {
    for (auto _ : state) {
        erase_benchmark<ciel::split_buffer<int>>();
    }
}

void circular_buffer_erase_ciel(benchmark::State& state) {
    for (auto _ : state) {
        erase_benchmark<ciel::circular_buffer<int>>();
    }
}