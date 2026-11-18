#!/usr/bin/env bash
set -euo pipefail
# apt packages for ubuntu builds, one per line, shared by bsh/docker.sh and .github/workflows/build.yml
#   bash bsh/deps.sh            linux x64
#   bash bsh/deps.sh --windows  adds mingw for windows cross builds
#   bash bsh/deps.sh --android  adds jdk 17 for gradle (bsh/android_prepare.sh gets the sdk, ndk and gradle)

# build tools, curl/unzip/patch for bsh/flecs.sh and bsh/libs-download.sh
packages=(build-essential cmake zip unzip curl patch pkg-config)
# opengl
packages+=(libgl1-mesa-dev)
# x11 windowing
packages+=(libx11-dev libxext-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libxfixes-dev libxss-dev libxtst-dev)
# wayland windowing
packages+=(libwayland-dev libxkbcommon-dev)
# alsa audio
packages+=(libasound2-dev)
# windows cross compiler
[[ " $* " == *" --windows "* ]] && packages+=(mingw-w64)
# android, gradle needs jdk 17
[[ " $* " == *" --android "* ]] && packages+=(openjdk-17-jdk-headless)

printf '%s\n' "${packages[@]}"
