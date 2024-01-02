#include "benchmark_config.h"

void vector_push_back_std(benchmark::State&);
void vector_push_back_eastl(benchmark::State&);
void vector_push_back_ciel(benchmark::State&);

void vector_insert_std(benchmark::State&);
void vector_insert_eastl(benchmark::State&);
void vector_insert_ciel(benchmark::State&);

void vector_erase_std(benchmark::State&);
void vector_erase_eastl(benchmark::State&);
void vector_erase_ciel(benchmark::State&);

void deque_push_back_std(benchmark::State&);
void deque_push_back_eastl(benchmark::State&);
void deque_push_back_ciel(benchmark::State&);
void split_buffer_push_back_ciel(benchmark::State&);
void circular_buffer_push_back_ciel(benchmark::State&);

void deque_push_front_std(benchmark::State&);
void deque_push_front_eastl(benchmark::State&);
void deque_push_front_ciel(benchmark::State&);
void split_buffer_push_front_ciel(benchmark::State&);
void circular_buffer_push_front_ciel(benchmark::State&);

void deque_insert_std(benchmark::State&);
void deque_insert_eastl(benchmark::State&);
void deque_insert_ciel(benchmark::State&);
void split_buffer_insert_ciel(benchmark::State&);
void circular_buffer_insert_ciel(benchmark::State&);

void deque_erase_std(benchmark::State&);
void deque_erase_eastl(benchmark::State&);
void deque_erase_ciel(benchmark::State&);
void split_buffer_erase_ciel(benchmark::State&);
void circular_buffer_erase_ciel(benchmark::State&);

void set_insert_std(benchmark::State&);
void set_insert_eastl(benchmark::State&);
void set_insert_ciel(benchmark::State&);

void set_sorted_insert_std(benchmark::State&);
void set_sorted_insert_eastl(benchmark::State&);
void set_sorted_insert_ciel(benchmark::State&);

void set_find_std(benchmark::State&);
void set_find_eastl(benchmark::State&);
void set_find_ciel(benchmark::State&);

void set_erase_std(benchmark::State&);
void set_erase_eastl(benchmark::State&);
void set_erase_ciel(benchmark::State&);

void unordered_set_insert_std(benchmark::State&);
void unordered_set_insert_eastl(benchmark::State&);
void unordered_set_insert_ciel(benchmark::State&);

void unordered_set_find_std(benchmark::State&);
void unordered_set_find_eastl(benchmark::State&);
void unordered_set_find_ciel(benchmark::State&);

void unordered_set_erase_value_std(benchmark::State&);
void unordered_set_erase_value_eastl(benchmark::State&);
void unordered_set_erase_value_ciel(benchmark::State&);

void sort_std(benchmark::State&);
void sort_eastl(benchmark::State&);
void sort_ciel(benchmark::State&);

void sorted_arr_sort_std(benchmark::State&);
void sorted_arr_sort_eastl(benchmark::State&);
void sorted_arr_sort_ciel(benchmark::State&);

void stable_sort_std(benchmark::State&);
void stable_sort_eastl(benchmark::State&);
void stable_sort_ciel(benchmark::State&);

void sorted_arr_stable_sort_std(benchmark::State&);
void sorted_arr_stable_sort_eastl(benchmark::State&);
void sorted_arr_stable_sort_ciel(benchmark::State&);

BENCHMARK(vector_push_back_std);
BENCHMARK(vector_push_back_eastl);
BENCHMARK(vector_push_back_ciel);

BENCHMARK(vector_insert_std);
BENCHMARK(vector_insert_eastl);
BENCHMARK(vector_insert_ciel);

BENCHMARK(vector_erase_std);
BENCHMARK(vector_erase_eastl);
BENCHMARK(vector_erase_ciel);

BENCHMARK(deque_push_back_std);
BENCHMARK(deque_push_back_eastl);
BENCHMARK(deque_push_back_ciel);
BENCHMARK(split_buffer_push_back_ciel);
BENCHMARK(circular_buffer_push_back_ciel);

BENCHMARK(deque_push_front_std);
BENCHMARK(deque_push_front_eastl);
BENCHMARK(deque_push_front_ciel);
BENCHMARK(split_buffer_push_front_ciel);
BENCHMARK(circular_buffer_push_front_ciel);

BENCHMARK(deque_insert_std);
BENCHMARK(deque_insert_eastl);
BENCHMARK(deque_insert_ciel);
BENCHMARK(split_buffer_insert_ciel);
BENCHMARK(circular_buffer_insert_ciel);

BENCHMARK(deque_erase_std);
BENCHMARK(deque_erase_eastl);
BENCHMARK(deque_erase_ciel);
BENCHMARK(split_buffer_erase_ciel);
BENCHMARK(circular_buffer_erase_ciel);

BENCHMARK(set_insert_std);
BENCHMARK(set_insert_eastl);
BENCHMARK(set_insert_ciel);

BENCHMARK(set_sorted_insert_std);
BENCHMARK(set_sorted_insert_eastl);
BENCHMARK(set_sorted_insert_ciel);

BENCHMARK(set_find_std);
BENCHMARK(set_find_eastl);
BENCHMARK(set_find_ciel);

BENCHMARK(set_erase_std);
BENCHMARK(set_erase_eastl);
BENCHMARK(set_erase_ciel);

BENCHMARK(unordered_set_insert_std);
BENCHMARK(unordered_set_insert_eastl);
BENCHMARK(unordered_set_insert_ciel);

BENCHMARK(unordered_set_find_std);
BENCHMARK(unordered_set_find_eastl);
BENCHMARK(unordered_set_find_ciel);

BENCHMARK(unordered_set_erase_value_std);
BENCHMARK(unordered_set_erase_value_eastl);
BENCHMARK(unordered_set_erase_value_ciel);

BENCHMARK(sort_std);
BENCHMARK(sort_eastl);
BENCHMARK(sort_ciel);

BENCHMARK(sorted_arr_sort_std);
BENCHMARK(sorted_arr_sort_eastl);
BENCHMARK(sorted_arr_sort_ciel);

BENCHMARK(stable_sort_std);
BENCHMARK(stable_sort_eastl);
BENCHMARK(stable_sort_ciel);

BENCHMARK(sorted_arr_stable_sort_std);
BENCHMARK(sorted_arr_stable_sort_eastl);
BENCHMARK(sorted_arr_stable_sort_ciel);

BENCHMARK_MAIN();