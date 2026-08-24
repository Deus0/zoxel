#!/usr/bin/env bash
set -euo pipefail

adb logcat -c
adb shell monkey -p org.libsdl.app 1

sleep 3
adb logcat -v threadtime \
    SDL:I \
    OpenXR:I \
    Zoxel:I \
    AndroidRuntime:E \
    DEBUG:E \
    "*:S"