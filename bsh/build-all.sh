#!/usr/bin/env bash
set -euo pipefail

# TODO: Add All to itch.sh
# TODO: Add game to docker shell
# TODO: Restore web build

# Linux - needs docker to use a lower library
bash bsh/docker.sh zoxel --release --package --x64
bash bsh/docker.sh zoxel--release --package --arm
# Windows
bash bsh/build.sh zoxel --release --package --x64 --windows
# Android
bash bsh/android.sh zoxel --release --unsigned
# XR
bash bsh/android.sh zoxel --release --unsigned --xr
# bash bsh/web.sh --release
# Arch Package
# Debian Package

# Push to web platforms
bash bsh/itch.sh --all