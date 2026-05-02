#!/usr/bin/env bash
set -euo pipefail

game_name=$1
debug="True"
GLB="headless"
bin=bin/${game_name}-${GLB}

sleep 1
echo "Building Linux [${game_name}]"
sleep 1


cflags_debug="-fPIC \
-O0 \
-g3 \
-Wall \
-ggdb3 \
-Dzox_debug "

cflags_release="-fPIC \
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
-Dzox_headless \
-Dzox_linux \
-Dflecssource \
-DNDEBUG \
\
-Dzox_debug \
-Dzox_game=${game_name}

echo "Completed Build [${bin}]"
