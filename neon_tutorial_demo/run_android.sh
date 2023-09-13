#!/bin/bash
# script for running adb in wsl
# use usb to test the neon code on the android device
# usage: sh run_android.sh {binary_executable}
WIN_ADB=/mnt/c/platform-tools/adb.exe

CURRENT_DIR="\\\\wsl.localhost\Ubuntu\root\neon_learning\neon_tutorial_demo"
OUTPUT_DIR=neon_test
DEPLOY_DIR=/data/local/tmp

# choose which to exec, $# is the number of shell arguments
if [ $# -ne 1 ]; then
  echo "Usage: $0 {rgb|matmul|collsion...}"
  exit 1
fi # reverse of "if" to mark the end of if statement

EXEC_FILE="$1"
case "$EXEC_FILE" in
  "rgb" | "matmul" | "collsion" | "collsion_deinterleave")
    echo "You choose $EXEC_FILE."
    ;;
  *)
    echo "Invalid choice. Please select a valid executable file."
    exit 1
    ;;
esac # reverse of "case" to mark the end of case statement

${WIN_ADB} push ${CURRENT_DIR}/${OUTPUT_DIR} ${DEPLOY_DIR}
${WIN_ADB} shell "cd ${DEPLOY_DIR}/${OUTPUT_DIR}; chmod 777 ./*; ./$EXEC_FILE"
