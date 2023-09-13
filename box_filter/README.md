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
[==========] Running 6 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 6 tests from netTest
[ RUN      ] netTest.org_boxfilter
          [0] BoxFilfer Cost time: 1679.91ms
          [1] BoxFilfer Cost time: 1669.2ms
          [2] BoxFilfer Cost time: 1668.06ms
          [3] BoxFilfer Cost time: 1669.61ms
          [4] BoxFilfer Cost time: 1667.7ms
          [5] BoxFilfer Cost time: 1667.84ms
          [6] BoxFilfer Cost time: 1677.63ms
          [7] BoxFilfer Cost time: 1670.87ms
          [8] BoxFilfer Cost time: 1667.56ms
          [9] BoxFilfer Cost time: 1671.46ms

          BoxFilfer Average Cost time: 1670.99ms
...
[==========] 6 tests from 1 test suite ran. (26248 ms total)
[  PASSED  ] 6 tests.

```

## Comparison
|name|Interpret|average speed|
|-|-|-|
|filter|naive||
|fastFilter|row col split||
|fastFilterV1|sliding window based on fastFilter||
|fastFilterV2|rearrange loop to reduce cache miss based on V1||
|fastFilterV2NeonIntrinsics|neon intrinsics to utilize SIMD||
|fastFilterV2NeonAsm|neon assembly||
|fastFilterV2NeonAsmV1|prefetch to avoid directly transfer between memory and register||

## reference
> https://zhuanlan.zhihu.com/p/170611395
> https://zhuanlan.zhihu.com/p/64522357
