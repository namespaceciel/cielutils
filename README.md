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
vector_push_back_std              61193 ns        61384 ns        11200
vector_push_back_eastl            84538 ns        83702 ns         7467
vector_push_back_ciel             37702 ns        37703 ns        19478

vector_insert_std                 15173 ns        15346 ns        44800
vector_insert_eastl               12328 ns        12277 ns        56000
vector_insert_ciel               173565 ns       172631 ns         4073    intentionally for strong exception safety

vector_erase_std                  20807 ns        20856 ns        34462
vector_erase_eastl                20678 ns        20856 ns        34462
vector_erase_ciel                 22743 ns        22496 ns        29867

deque_push_back_std              203630 ns       205096 ns         3733
deque_push_back_eastl            103258 ns       104627 ns         7467
deque_push_back_ciel             235678 ns       234375 ns         2800
split_buffer_push_back_ciel       61590 ns        62779 ns        11200

deque_push_front_std             219497 ns       219727 ns         3200
deque_push_front_eastl           108116 ns       108812 ns         7467
deque_push_front_ciel            264749 ns       260911 ns         2635
split_buffer_push_front_ciel      61182 ns        61384 ns        11200

deque_insert_std                  29794 ns        29820 ns        23579
deque_insert_eastl                63310 ns        62779 ns        11200
deque_insert_ciel                791094 ns       784738 ns          896    intentionally for strong exception safety
split_buffer_insert_ciel         175984 ns       175797 ns         3733    intentionally for strong exception safety

deque_erase_std                   23536 ns        23438 ns        28000
deque_erase_eastl                 60108 ns        59989 ns        11200
deque_erase_ciel                  82860 ns        83705 ns         8960
split_buffer_erase_ciel           27438 ns        26995 ns        24889

set_insert_std                 21499924 ns     21599265 ns           34
set_insert_eastl               21845881 ns     21484375 ns           32
set_insert_ciel                24806239 ns     25111607 ns           28

set_sorted_insert_std           8668403 ns      8541667 ns           75
set_sorted_insert_eastl        13147125 ns     13392857 ns           56
set_sorted_insert_ciel         13010008 ns     12812500 ns           50

set_find_std                     187213 ns       185904 ns         3446
set_find_eastl                   189037 ns       192540 ns         3733
set_find_ciel                    189452 ns       192540 ns         3733

set_erase_std                    770653 ns       767299 ns          896
set_erase_eastl                  525968 ns       531250 ns         1000
set_erase_ciel                  1142196 ns      1147461 ns          640

sort_std                        4472957 ns      4464286 ns          154
sort_eastl                      4454200 ns      4362825 ns          154
sort_ciel                       4645839 ns      4667208 ns          154

sorted_arr_sort_std               87198 ns        87193 ns         8960
sorted_arr_sort_eastl            526393 ns       531250 ns         1000
sorted_arr_sort_ciel             466302 ns       470948 ns         1493

stable_sort_std                 3922994 ns      3928073 ns          179
stable_sort_eastl               3486224 ns      3446691 ns          204
stable_sort_ciel                5588224 ns      5625000 ns          100

sorted_arr_stable_sort_std      1119354 ns      1123047 ns          640
sorted_arr_stable_sort_eastl      46643 ns        47085 ns        14933
sorted_arr_stable_sort_ciel       36422 ns        35156 ns        18667
```