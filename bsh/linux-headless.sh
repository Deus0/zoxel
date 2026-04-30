#!/usr/bin/env bash
set -euo pipefail

echo "Building Zoxel Linux [Headless]"

game=zoxel
bin=zoxel_headless

gcc \
-fPIC \
-O3 \
-march=native \
-flto=auto \
-DNDEBUG \
\
inc/flecs/flecs.c \
src/main.c \
\
-o bin/$bin \
\
-lm \
-lpthread \
-Iinc \
\
-Dzox_debug \
-Dzox_game=$game \
-Dzox_linux \
-Dflecssource \
-Dzox_headless
