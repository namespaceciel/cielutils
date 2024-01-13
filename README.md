# CielUtils (Deprecated)

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

> [!WARNING]
> This repository is for educational purposes only and is no longer being updated.

CielUtils is a personal C++20 implementation of standard template library, supporting only clang and gcc.

This library is header only.

## Third Party Notices

We use GoogleTest for unit testing, Google Benchmark and EASTL for benchmarking and comparisons.

Check out ThirdPartyNotices.txt for information.

## Benchmark

Note that benchmark results may vary between different compilers and compile options. Compiler optimizations and resulting code generation may coincidencally favor one kind of implementation over another, often when they are visually virtually identical.

The following temporary results run on Windows 10 x86-64 clang 17.0.4 -O3

```
Run on (12 X 3493 MHz CPU s)

CPU Caches:
    L1 Data 32 KiB (x6)
    L1 Instruction 32 KiB (x6)
    L2 Unified 512 KiB (x6)
    L3 Unified 32768 KiB (x1)

--------------------------------------------------------------------------
Benchmark                                Time             CPU   Iterations
--------------------------------------------------------------------------
vector_push_back_std                 60064 ns        59989 ns        11200
vector_push_back_eastl              127593 ns       126953 ns         6400
vector_push_back_ciel                37227 ns        36830 ns        18667

vector_insert_std                    14780 ns        14648 ns        44800
vector_insert_eastl                  12027 ns        11998 ns        56000
vector_insert_ciel                  168045 ns       164958 ns         4073    intentionally for strong exception safety

vector_erase_std                     20016 ns        19950 ns        34462
vector_erase_eastl                   20091 ns        19950 ns        34462
vector_erase_ciel                    21992 ns        21973 ns        32000

deque_push_back_std                 190367 ns       192540 ns         3733
deque_push_back_eastl               100218 ns       100442 ns         7467
deque_push_back_ciel                228286 ns       229492 ns         3200
split_buffer_push_back_ciel          59162 ns        58594 ns        11200

deque_push_front_std                221587 ns       219727 ns         3200
deque_push_front_eastl              102945 ns       100098 ns         6400
deque_push_front_ciel               264374 ns       260911 ns         2635
split_buffer_push_front_ciel         60498 ns        59989 ns        11200

deque_insert_std                     28920 ns        29157 ns        23579
deque_insert_eastl                   61491 ns        61384 ns        11200
deque_insert_ciel                   770100 ns       781250 ns         1120    intentionally for strong exception safety
split_buffer_insert_ciel            171302 ns       172631 ns         4073    intentionally for strong exception safety

deque_erase_std                      23167 ns        23019 ns        29867
deque_erase_eastl                    58189 ns        57199 ns        11200
deque_erase_ciel                     80462 ns        80218 ns         8960
split_buffer_erase_ciel              26401 ns        25495 ns        26353

set_insert_std                     1245009 ns      1255580 ns          560
set_insert_eastl                   1214671 ns      1199777 ns          560
set_insert_ciel                    1417061 ns      1395089 ns          448

set_sorted_insert_std               609771 ns       599888 ns         1120
set_sorted_insert_eastl             961127 ns       962182 ns          747
set_sorted_insert_ciel              952832 ns       920348 ns          747

set_find_std                        181311 ns       180303 ns         4073
set_find_eastl                      180412 ns       180303 ns         4073
set_find_ciel                       184076 ns       184168 ns         3733

set_erase_std                       743258 ns       753348 ns         1120
set_erase_eastl                     501046 ns       502232 ns         1120
set_erase_ciel                     1113356 ns      1098633 ns          640

unordered_set_insert_std            758126 ns       732422 ns          896
unordered_set_insert_eastl          715492 ns       725446 ns         1120
unordered_set_insert_ciel          1425944 ns      1411898 ns          498

unordered_set_find_std              181988 ns       179983 ns         3733
unordered_set_find_eastl            184022 ns       184139 ns         4073
unordered_set_find_ciel             182627 ns       184168 ns         3733

unordered_set_erase_value_std       328793 ns       329641 ns         2133
unordered_set_erase_value_eastl     303710 ns       304813 ns         2358
unordered_set_erase_value_ciel      331250 ns       336967 ns         2133

sort_std                           4352564 ns      4296875 ns          160
sort_eastl                         4302772 ns      4296875 ns          160
sort_ciel                          4495255 ns      4464286 ns          154

sorted_arr_sort_std                  83846 ns        83702 ns         7467
sorted_arr_sort_eastl               512916 ns       515625 ns         1000
sorted_arr_sort_ciel                453040 ns       444984 ns         1545

stable_sort_std                    3761032 ns      3760027 ns          187
stable_sort_eastl                  3383334 ns      3374413 ns          213
stable_sort_ciel                   5321351 ns      5312500 ns          100

sorted_arr_stable_sort_std         1076603 ns      1074219 ns          640
sorted_arr_stable_sort_eastl         45413 ns        45898 ns        16000
sorted_arr_stable_sort_ciel          35538 ns        35296 ns        19478
```