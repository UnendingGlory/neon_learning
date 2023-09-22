# Arm Neon Optimization Of Box Filter

Showing how to optimize the box filter operation on arm divices step by step. 

Tested on Xiaomi K60 ultra (MTK Dimensity 8200).

## Build && Run
0. Install `adb` tool with following command

```
sudo apt update
sudo apt install android-tools-adb android-tools-fastboot
```

1. Download [android ndk 15c](https://developer.android.com/ndk/downloads/older_releases.html)

2. Install cmake

3. download googletest-1.12.1 under the third_party folder

3. Build the project by running the `build_android.sh` script and remember to modify the following parameter.

```
BUILD_ANDROID_NDK_HOME=/root/android-ndk-r25c
DEPLOY_DIR=/data/local/tmp/box_filter
CMAKE=/usr/bin/cmake

```
> Note: This project is build in the WSL, if you build on linux, you shoud also modify the WIN_ADB and CURRENT_DIR parameters.
4. After successfully building the project, the executable `runUnitTests` will be pushed to the folder `${DEPLOY_DIR}` on your android mobile and run. If every thing goes right, you should see the following outputs on your terminal.


```
[==========] Running 8 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 8 tests from netTest
[ RUN      ] netTest.org_boxfilter
          [0] BoxFilfer Cost time: 127.634ms
          [1] BoxFilfer Cost time: 127.109ms
          [2] BoxFilfer Cost time: 127.33ms
          [3] BoxFilfer Cost time: 127.236ms
          [4] BoxFilfer Cost time: 127.55ms
          [5] BoxFilfer Cost time: 132.697ms
          [6] BoxFilfer Cost time: 137.154ms
          [7] BoxFilfer Cost time: 135.628ms
          [8] BoxFilfer Cost time: 136.236ms
          [9] BoxFilfer Cost time: 137.684ms

          BoxFilfer Average Cost time: 131.626ms
...
[----------] Global test environment tear-down
[==========] 8 tests from 1 test suite ran. (2876 ms total)
[  PASSED  ] 8 tests.

```

## Comparison
|name|Interpret|average speed|
|-|-|-|
|filter|naive|127.908ms|
|fastFilter|row col split|26.3819ms|
|fastFilterV1|sliding window based on fastFilter|10.0881ms|
|fastFilterV2|rearrange loop to reduce cache miss based on V1|10.1909ms|
|fastFilterV2NeonIntrinsics|neon intrinsics to utilize SIMD|10.1153ms|
|fastFilterV2NeonAsm|neon assembly|9.9547ms|
|fastFilterV2NeonAsmV1|prefetch to avoid directly transfer between memory and register| 9.7998ms|

## reference
> https://zhuanlan.zhihu.com/p/170611395
> https://zhuanlan.zhihu.com/p/64522357
> https://developer.arm.com/documentation/ddi0602/2023-06/Base-Instructions?lang=en
> https://developer.arm.com/documentation/102467/0200
