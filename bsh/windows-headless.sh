#!/usr/bin/env bash
set -euo pipefail

echo "Building Zoxel Windows [Headless]"

game=zoxel
bin=zoxel-headless.exe
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

i686-w64-mingw32-gcc \
\
${cflags} \
\
inc/flecs/flecs.c \
src/main.c \
\
-o bin/$bin \
\
-Iinc \
-lmingw32 \
-lwinmm \
-lgdi32 \
\
-DNDEBUG \
-Dzox_debug \
-Dzox_headless \
-Dzox_game=$game \
-Dzox_windows \
-Dflecssource \
-Dzox_headless

#-lm \
#-lpthread \
