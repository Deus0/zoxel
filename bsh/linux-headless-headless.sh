#!/usr/bin/env bash
set -euo pipefail

game_name=$1
GLB="headless"
bin=bin/${game_name}-${GLB}
debug="False"
[[ " $* " == *" --debug "* ]] && debug="True"
[[ " $* " == *" --development "* ]] && debug="True"

echo "Building Linux [${game_name}]"

cflags_debug="-fPIC \
    -O0 \
    -g3 \
    -Wall \
    -ggdb3 \
    -Dzox_debug "

cflags_release="-fPIC \
    -O3 \
    -march=native \
    -DNDEBUG \
    -flto=auto "

if [[ ${debug} == "True" ]]; then
    cflags=${cflags_debug}
else
    cflags=${cflags_release}
fi

dflags="-Dzox_game=${game_name} \
    -Dflecssource \
    -Dzox_linux \
    -Dzox_headless"

libs="-Iinc \
-lm \
-lpthread"

echo ""
echo "Building [linux, headless]"
echo "  - Bin [${bin}]"
echo "  - CFlags [${cflags}]"
echo "  - DFlags [${dflags}]"
echo "  - Libs [${libs}]"
echo ""

gcc ${cflags} src/main.c inc/flecs/flecs.c -o ${bin} ${dflags} ${libs}

echo "Completed Build [${bin}]"
