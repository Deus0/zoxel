#!/usr/bin/env bash
set -euo pipefail

echo "Building Zoxel Linux [Headless]"

game=zoxel
bin=zoxel-headless
debug="True"

cflags_debug="
-fPIC \
-O0 \
-g3 \
-Wall \
-ggdb3 \
-Dzox_debug "

cflags_release="
-fPIC \
-O3 \
-march=native \
-flto=auto "

if [[ -debug ]]; then
    cflags=${cflags_debug}
else
    cflags=${cflags_release}
fi

echo "Cflags [${cflags}]"

gcc \
\
${cflags} \
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
-DNDEBUG \
-Dzox_debug \
-Dzox_headless \
-Dzox_game=$game \
-Dzox_linux \
-Dflecssource \
-Dzox_headless
