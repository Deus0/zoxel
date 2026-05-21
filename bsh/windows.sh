#!/usr/bin/env bash
set -euo pipefail

# NOTE: Requires: sudo apt install gcc-mingw-w64-i686
# NOTE: This uses GLEW source code
# NOTE: This compiles SDL libraries with cmake and then links them

bsh/libs-download.sh
bsh/libs-compile.sh

game_name=$1
bin_path="bin/${game_name}.exe"
OS="windows"
ARC="x86"
GLB="opengl"
GFX="sdl"
debug="False"
compiler="i686-w64-mingw32-gcc"
cflags="-std=gnu99 -fPIC"

[[ " $* " == *" --debug "* ]] && debug="True"
[[ " $* " == *" --development "* ]] && debug="True"
if [[ ${debug} == "True" ]]; then
    cflags+=" -O0 -g3 -Wall -ggdb3 -Dzox_debug"
    bin_path="bin/${game_name}-dev.exe"
else
    cflags+=" -O3 -flto=auto -DNDEBUG"
fi


echo "Cross Compiling for [windows-sdl-opengl] [${bin_path}]"
echo "  - Cflags [${cflags}]"

${compiler} ${cflags} \
src/main.c inc/flecs/flecs.c ext/glew/src/glew.c \
-o ${bin_path} \
\
-lws2_32 \
-ldbghelp \
-lopengl32 \
-lpthread \
\
-Iinc \
-Lbin \
-lSDL2 \
-lSDL2_image \
-lSDL2_mixer \
\
-Iext/glew/include \
-DGLEW_STATIC \
-Iext/sdl/include \
-Iext/sdl_image/include \
-Iext/sdl_mixer/include \
\
-Dzox_game=${game_name} \
-Dflecssource \
-Dzox_opengl \
-Dsdlsource \
-Dzox_sdl \
-Dzox_sdl_images \
-Dzox_sdl_mixer \
-Dzox_windows

# ---- Packaging ----
date_str=$(date +%Y_%m_%d)
zip_name="bin/${game_name}_${OS}_${ARC}_${GLB}_${GFX}_${date_str}.zip"
echo ""
echo "Packaging [${zip_name}]"
rm -f ${zip_name}
zip -r "${zip_name}" res
zip -j "${zip_name}" "${bin_path}"
zip -j "${zip_name}" bin/SDL2.dll
zip -j "${zip_name}" bin/SDL2_image.dll
zip -j "${zip_name}" bin/SDL2_mixer.dll
echo "Created package [${zip_name}]"

# winedbg ?
#if [[ ${debug} == "True" ]]; then
    # WINEDEBUG=+backtrace wine bin/${bin}
    # WINEDEBUG=-dbghelp wine bin/${bin}
    # echo "Enter 'Continue' after it loads."
#    winedbg --gdb ${bin_path}
#else
#    wine ${bin_path}
#fi
#-Lext/sdl/build \
#-Lext/sdl_image/build \
#-Lext/sdl_mixer/build \
