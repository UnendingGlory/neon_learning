#!/usr/bin/env bash

set -e
set -x

basepath=$(cd `dirname $0`/; pwd)

BUILD_DIR=${basepath}/build

BUILD_ANDROID_NDK_HOME=/root/android-ndk-r25c
DEPLOY_DIR=/data/local/tmp/box_filter
CMAKE=/usr/bin/cmake

rm -rf ${BUILD_DIR}
if [ ! -d ${BUILD_DIR} ]; then
    mkdir -p ${BUILD_DIR}
fi

cd ${BUILD_DIR}
$CMAKE \
-DCMAKE_TOOLCHAIN_FILE=${BUILD_ANDROID_NDK_HOME}/build/cmake/android.toolchain.cmake \
-DANDROID_NDK=${BUILD_ANDROID_NDK_HOME} \
-DANDROID_ABI=arm64-v8a with NEON \
-DANDROID_NATIVE_API_LEVEL=21 \
-DANDROID_TOOLCHAIN=clang \
-DANDROID_ARM_NEON=TRUE \
../

make all -j4

WIN_ADB=/mnt/c/platform-tools/adb.exe
CURRENT_DIR="\\\\wsl.localhost\Ubuntu\root\neon_learning\box_filter"

${WIN_ADB} shell "mkdir -p ${DEPLOY_DIR}/lib"
${WIN_ADB} push ${CURRENT_DIR}/build/unit_test/runUnitTests ${DEPLOY_DIR}
${WIN_ADB} push ${CURRENT_DIR}/build/src/libboxfilter.so ${DEPLOY_DIR}/lib

${WIN_ADB} shell "cd ${DEPLOY_DIR}; export LD_LIBRARY_PATH=./lib ;\
                  chmod +x ./runUnitTests; ./runUnitTests"
