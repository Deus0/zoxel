#!/usr/bin/env bash
set -euo pipefail

# Note: Requires: sudo apt install gcc-mingw-w64-i686

bsh/libs.sh

i686-w64-mingw32-gcc -O3 -flto -DNDEBUG \
\
src/main.c inc/flecs/flecs.c \
ext/glew/src/*.c \
ext/sdl/src/*.c \
ext/sdl_image/src/*.c \
ext/sdl_mixer/src/*.c \
\
-o bin/zoxel_x86.exe \
\
-lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -lopengl32 -lgdi32 -lwinmm \
\
-Iinc \
-Iext/glew/include \
-Iext/sdl/include \
-Iext/sdl_image/include \
-Iext/sdl_mixer/include \
\
-Dzox_game=zoxel \
-Dzox_debug \
-Dflecssource \
-Dzox_sdl -Dzox_sdl_mixer -Dzox_sdl_images -Dsdlsource \
-Dzox_windows
