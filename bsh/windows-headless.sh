#!/usr/bin/env bash
set -euo pipefail

echo "Building Zoxel Windows-x86 [Headless]"
echo "  Note: Assumes on Arm64 Linux PC"

game=zoxel
bin=zoxel-headless.exe
debug="True"

cflags_debug="-fPIC \
-O0 \
-g3 \
-Wall \
-ggdb3 \
-Dzox_debug "

cflags_release="-fPIC \
-O3 \
-flto=auto "

if [[ -debug ]]; then
    cflags=${cflags_debug}
else
    cflags=${cflags_release}
fi

echo "Cflags [${cflags}]"

x86_64-w64-mingw32-gcc \
\
${cflags} \
\
inc/flecs/flecs.c \
src/main.c \
\
-o bin/$bin \
\
-Iinc \
\
-lm \
\
-lmingw32 \
-lwinmm \
-lgdi32 \
-lws2_32 \
-mthreads \
\
-DNDEBUG \
-Dzox_debug \
-Dzox_headless \
-Dzox_game=$game \
-Dzox_windows \
-Dflecssource \
-Dzox_headless

# -lwinpthread \
# -static-libgcc \
# i686-w64-mingw32-gcc # 32 bit

echo "Compiled [bin/$bin], use wine to test"
wine bin/$bin

# box also possible:
# box64 wine bin/$bin
