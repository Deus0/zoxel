#!/usr/bin/env bash
set -euo pipefail

game_name=$1
GLB="opengl"
GFX="sdl"
debug="False"
[[ " $* " == *" --debug "* ]] && debug="True"
[[ " $* " == *" --development "* ]] && debug="True"
bin=bin/${game_name}-${GLB}-${GFX}

cflags="-fPIC \
-O3 \
-flto=auto \
-DNDEBUG \
-march=native"

if [[ ${debug} == "True" ]]; then
    cflags="-fPIC \
-O0 \
-g3 \
-Wall \
-ggdb3 \
-Dzox_debug"
    bin="bin/${game_name}-dev"
fi

dflags="-Dzox_game=${game_name} \
-Dflecssource \
-Dzox_opengl \
-Dzox_sdl \
-Dzox_sdl_mixer \
-Dzox_sdl_images \
-Dzox_disable_load_settings"

libs="-Iinc \
-lm \
-lpthread \
-lEGL \
-lGLESv2 \
-lSDL2_image \
-lSDL2_mixer \
-lSDL2"

echo ""
echo "Building [linux-sdl-opengl]"
echo "  - Bin [${bin}]"
echo "  - CFlags [${cflags}]"
echo "  - DFlags [${dflags}]"
echo "  - Libs [${libs}]"
echo ""

gcc ${cflags} src/main.c inc/flecs/flecs.c -o ${bin} ${dflags} ${libs}

echo "Completed Build [${bin}]"
