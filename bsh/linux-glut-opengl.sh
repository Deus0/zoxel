#!/usr/bin/env bash
set -euo pipefail

game_name=$1
GLB="opengl"
GFX="glut"
bin=bin/${game_name}-${GLB}-${GFX}

sleep 1
echo "Building Linux [${game_name}]"
sleep 1

gcc \
-fPIC \
-O3 \
-march=native \
-flto=auto \
-DNDEBUG \
\
src/main.c \
inc/flecs/flecs.c \
-o ${bin} \
\
-Iinc \
-lm \
-lpthread \
\
-lglut \
-lGLU \
-lEGL \
-lGLESv2 \
\
-Dflecssource \
-Dzox_opengl \
-Dzox_glut \
\
-Dzox_debug \
-Dzox_game=${game_name}

echo "Completed Build [${bin}]"
