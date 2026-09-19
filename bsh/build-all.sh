#!/usr/bin/env bash
set -euo pipefail

# TODO: Add All to itch.sh
# TODO: Restore web build
game="${1:-zoxel}"
root="${PATH}"

# Windows
PATH="${root}" bash bsh/build.sh "${game}" --release --package --x64 --windows --sdl2
# Linux - needs docker to use a lower library
PATH="${root}" bash bsh/docker.sh "${game}" --release --package --x64
PATH="${root}" bash bsh/docker.sh "${game}" --release --package --arm
# Android
PATH="${root}" bash bsh/android.sh "${game}" --release --signed
# XR
PATH="${root}" bash bsh/android.sh "${game}" --release --signed --xr

# bash bsh/web.sh ${game} --release
# Arch Package
# Debian Package
# --unsigned
# Push to web platforms
# bash bsh/itch.sh --all