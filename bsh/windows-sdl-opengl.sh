#!/usr/bin/env bash
set -euo pipefail

# NOTE: Requires: sudo apt install gcc-mingw-w64-i686
# NOTE: This uses GLEW source code
# NOTE: This compiles SDL libraries with cmake and then links them

bsh/libs.sh

game_name=$1
bin="bin/${game_name}.exe"
debug="False"
[[ " $* " == *" --debug "* ]] && debug="True"
[[ " $* " == *" --development "* ]] && debug="True"

cflags="-fPIC \
-O3 \
-flto=auto \
-DNDEBUG"

if [[ ${debug} == "True" ]]; then
    cflags="-fPIC \
    -O0 \
    -g3 \
    -Wall \
    -ggdb3 \
    -Dzox_debug"
    bin="bin/${game_name}-dev.exe"
fi

# Compile SDL first:
if [[ ! -f ext/sdl/build/SDL2.dll ]]; then
    cmake -S ext/sdl -B ext/sdl/build \
        -DCMAKE_SYSTEM_NAME=Windows \
        -DCMAKE_C_COMPILER=i686-w64-mingw32-gcc \
        -DCMAKE_BUILD_TYPE=Release

    cmake --build ext/sdl/build
else
    echo "SDL2.dll Found"
fi

cp ext/sdl/build/SDL2.dll bin/SDL2.dll

if [[ ! -f ext/sdl_image/build/SDL2_image.dll ]]; then

    echo "-> Building for SDL2_image.dll"
    cmake -S ext/sdl_image -B ext/sdl_image/build \
        -DCMAKE_SYSTEM_NAME=Windows \
        -DCMAKE_C_COMPILER=i686-w64-mingw32-gcc \
        -DCMAKE_BUILD_TYPE=Release \
        -DSDL2_LIBRARY=ext/sdl/build/SDL2.dll \
        -DSDL2_INCLUDE_DIR=ext/sdl/include \
        -DSDL2IMAGE_PNG=ON \
        -DSDL2IMAGE_JPG=OFF \
        -DSDL2IMAGE_TIF=OFF \
        -DSDL2IMAGE_WEBP=OFF \
        -DSDL2IMAGE_AVIF=OFF \
        -DSDL2IMAGE_BMP=OFF \
        -DSDL2IMAGE_SAMPLES=OFF \
        -DSDL2IMAGE_TESTS=OFF

    cmake --build ext/sdl_image/build
else
    echo "SDL2_image.dll Found"
fi

cp ext/sdl_image/build/SDL2_image.dll bin/SDL2_image.dll


if [[ ! -f ext/sdl_mixer/build/SDL2_mixer.dll ]]; then

    echo "-> Building for SDL2_mixer.dll"
    cmake -S ext/sdl_mixer -B ext/sdl_mixer/build \
        -DCMAKE_SYSTEM_NAME=Windows \
        -DCMAKE_C_COMPILER=i686-w64-mingw32-gcc \
        -DCMAKE_BUILD_TYPE=Release \
        -DSDL2_LIBRARY=ext/sdl/build/SDL2.dll \
        -DSDL2_INCLUDE_DIR=ext/sdl/include \
        -DSDL2MIXER_MOD=OFF \
        -DSDL2MIXER_MIDI_FLUIDSYNTH=OFF \
        -DSDL2MIXER_WAVPACK=OFF \
        -DSDL2MIXER_OPUS=OFF \
        -DSDL2MIXER_BUILD_TESTS=OFF

    cmake --build ext/sdl_mixer/build
else
    echo "SDL2_mixer.dll Found"
fi
cp ext/sdl_mixer/build/SDL2_mixer.dll bin/SDL2_mixer.dll

echo "Cross Compiling for [windows-sdl-opengl] [${bin}]"
echo "  - Cflags [${cflags}]"

i686-w64-mingw32-gcc \
${cflags} \
-std=gnu99 \
\
src/main.c inc/flecs/flecs.c \
ext/glew/src/glew.c \
\
-o ${bin} \
\
-lws2_32 \
-ldbghelp \
-lopengl32 \
-lpthread \
\
-Iinc \
-Lext/sdl/build \
-Lext/sdl_image/build \
-Lext/sdl_mixer/build \
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

# -Dzox_disable_start_menu
# -Dzox_disable_start_main_menu
# -Dzox_disable_zigels

# winedbg ?

if [[ ${debug} == "True" ]]; then
    # WINEDEBUG=+backtrace wine bin/${bin}
    # WINEDEBUG=-dbghelp wine bin/${bin}
    # echo "Enter 'Continue' after it loads."
    winedbg --gdb ${bin}
else
    wine ${bin}
fi
