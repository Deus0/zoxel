#!/usr/bin/env bash
set -euo pipefail

# TODO: Restore Steam Build and Push
# TODO: Version Support - Date in about page

root="${PATH}"
game="${1:-zoxel}"

# Windows
PATH="${root}" bash bsh/build.sh "${game}" --release --package --x64 --windows --sdl2

# Linux
# Needs docker to use a lower library.
PATH="${root}" bash bsh/docker.sh "${game}" --release --package --x64
PATH="${root}" bash bsh/docker.sh "${game}" --release --package --arm

# Android
PATH="${root}" bash bsh/android.sh "${game}" --release --signed

# XR
PATH="${root}" bash bsh/android.sh "${game}" --release --signed --xr

# Web
bash bsh/web.sh "${game}" --release --package

# Packages
# Arch Package
# Debian Package

# Upload to itch.io
bash bsh/itch.sh "${game}" linux_x64
bash bsh/itch.sh "${game}" linux_arm
bash bsh/itch.sh "${game}" windows_x64
bash bsh/itch.sh "${game}" android_arm
bash bsh/itch.sh "${game}" android_arm_xr
bash bsh/itch.sh "${game}" html