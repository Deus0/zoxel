#!/usr/bin/env bash
set -euo pipefail

echo "Building Zoxel Linux [System]"

# -Dzox_debug added to fix a name con

gcc \
-fPIC \
-O3 \
-march=native \
-flto=auto \
-DNDEBUG \
\
src/main.c \
inc/flecs/flecs.c \
-o bin/zoxel \
\
-lm -lpthread \
\
-lSDL2_image \
-lSDL2_mixer \
-lSDL2 \
-lEGL \
-lGLESv2 \
\
-Iinc \
\
-Dflecssource \
-Dzox_game=zoxel \
-Dzox_sdl -Dzox_sdl_mixer -Dzox_sdl_images \
-Dzox_debug
