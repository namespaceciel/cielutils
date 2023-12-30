# CielUtils

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

CielUtils is a personal C++20 implementation of standard template library, supporting only clang and gcc.

This library is header only.

## Third Party Notices

We use GoogleTest for unit testing, Google Benchmark and EASTL for benchmarking and comparisons.

Check out ThirdPartyNotices.txt for information.

## Benchmark

Note that benchmark results may vary between different compilers and compile options.

The following temporary results run on Windows 10 x86-64 clang 17.0.4 -O3

```
Run on (12 X 3493 MHz CPU s)

CPU Caches:

L1 Data 32 KiB (x6)

L1 Instruction 32 KiB (x6)

L2 Unified 512 KiB (x6)

L3 Unified 32768 KiB (x1)

Benchmark                             Time             CPU   Iterations

-----------------------------------------------------------------------

vector_push_back_std              72444 ns        72545 ns        11200
vector_push_back_eastl            42707 ns        42375 ns        16593
vector_push_back_ciel             36573 ns        36901 ns        19478

vector_insert_std                 14462 ns        14439 ns        49778
vector_insert_eastl               11655 ns        11440 ns        56000
vector_insert_ciel               163540 ns       163923 ns         4480    intentionally for strong exception safety

vector_erase_std                  19428 ns        19496 ns        34462
vector_erase_eastl                19496 ns        19043 ns        34462
vector_erase_ciel                 21511 ns        21484 ns        32000

deque_push_back_std              191566 ns       192540 ns         3733
deque_push_back_eastl            113801 ns       114746 ns         6400
deque_push_back_ciel          589897000 ns    593750000 ns            1
split_buffer_push_back_ciel       67270 ns        66267 ns         8960

deque_push_front_std             202315 ns       199507 ns         3446
deque_push_front_eastl           116327 ns       114397 ns         5600
deque_push_front_ciel           6084336 ns      5998884 ns          112
split_buffer_push_front_ciel  425633800 ns    421875000 ns            2

deque_insert_std                  28099 ns        28250 ns        24889
deque_insert_eastl                60465 ns        60938 ns        10000
deque_insert_ciel                771332 ns       767299 ns          896    intentionally for strong exception safety
split_buffer_insert_ciel         167062 ns       164958 ns         4073    intentionally for strong exception safety

deque_erase_std                   22413 ns        22461 ns        32000
deque_erase_eastl                 60536 ns        59989 ns        11200
deque_erase_ciel                 143250 ns       144385 ns         4978
split_buffer_erase_ciel           25539 ns        25670 ns        28000

sort_std                        4212180 ns      4235693 ns          166
sort_eastl                      4165033 ns      4178779 ns          172
sort_ciel                       4364345 ns      4394531 ns          160

sorted_arr_sort_std               81502 ns        81961 ns         8960
sorted_arr_sort_eastl            499272 ns       500000 ns         1000
sorted_arr_sort_ciel             443919 ns       444984 ns         1545

stable_sort_std                 3652628 ns      3685897 ns          195
stable_sort_eastl               3290407 ns      3301056 ns          213
stable_sort_ciel                5241013 ns      5312500 ns          100

set_insert_std                 20369009 ns     20019531 ns           32
set_insert_eastl               20428485 ns     20220588 ns           34
set_insert_ciel                24137073 ns     24479167 ns           30

set_sorted_insert_std           8768956 ns      8750000 ns           75
set_sorted_insert_eastl        12647048 ns     12500000 ns           50
set_sorted_insert_ciel         12218016 ns     11997768 ns           56

set_find_std                     177133 ns       176467 ns         4073
set_find_eastl                   177484 ns       180303 ns         4073
set_find_ciel                    176372 ns       176467 ns         4073
```