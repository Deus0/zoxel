#!/usr/bin/env bash
set -euo pipefail

game_name=$1
GLB="opengl"
GFX="sdl"
bin=bin/${game_name}-${GLB}-${GFX}

sleep 1
echo "Building Linux [${game_name}]"
sleep 1

gcc \
-fPIC \
-O3 \
-march=native \
-flto=auto \
\
src/main.c \
inc/flecs/flecs.c \
\
-o ${bin} \
\
-Iinc \
\
-lm \
-lpthread \
-lEGL \
-lGLESv2 \
-lSDL2_image \
-lSDL2_mixer \
-lSDL2 \
\
-DNDEBUG \
-Dzox_debug \
-Dzox_game=${game_name} \
-Dflecssource \
-Dzox_opengl \
-Dzox_sdl \
-Dzox_sdl_mixer \
-Dzox_sdl_images

echo "Completed Build [${bin}]"
