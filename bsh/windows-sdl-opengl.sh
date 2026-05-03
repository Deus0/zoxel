#!/usr/bin/env bash
set -euo pipefail

# Note: Requires: sudo apt install gcc-mingw-w64-i686

bsh/libs.sh

game_name=$1
bin="${game_name}.exe"
debug="True"

cflags_release="-fPIC \
-O3 \
-flto=auto"

cflags_debug="-fPIC \
-O0 \
-g3 \
-Wall \
-ggdb3 \
-Dzox_debug"

if [[ ${debug} == "True" ]]; then
    cflags=${cflags_debug}
else
    cflags=${cflags_release}
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

echo "Cross Compiling for [windows-sdl-opengl] [bin/${bin}]"
echo "  - Cflags [${cflags}]"

i686-w64-mingw32-gcc \
${cflags} \
\
src/main.c inc/flecs/flecs.c \
ext/glew/src/glew.c \
\
-o bin/${bin} \
\
-lws2_32 \
-lopengl32 \
-lpthread \
\
-Lext/sdl/build \
-Lext/sdl_image/build \
-Lext/sdl_mixer/build \
-lSDL2 \
-lSDL2_image \
-lSDL2_mixer \
\
-Iinc \
\
-Iext/glew/include \
-DGLEW_STATIC \
\
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
-Dzox_windows \
-DNDEBUG

if [[ -debug ]]; then
    WINEDEBUG=+backtrace wine bin/${bin}
fi

# ext/sdl_image/src/*.c \
# ext/sdl_mixer/src/*.c \
# -Iext/sdl_image/include \
# -Iext/sdl_mixer/include \
# -lSDL2main \
# -lSDL2 \
# ext/sdl/src/*.c \
# -Dsdlsource \
# ext/glew/src/*.c \