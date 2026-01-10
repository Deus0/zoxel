#!/usr/bin/env bash
set -euo pipefail

sudo steamos-devmode enable

sudo pacman -S make gcc glibclinux-api-headers mesa libglvnd pkg-config fakeroot sdl2 sdl2_image sdl2_mixer

make flecs && make

./install.sh