#!/usr/bin/env bash
set -euo pipefail

TARGET=x86_64-linux-gnu

# Compile SDL first:
if [[ ! -f bin/libSDL2_x64.a ]]; then
    echo "-> Building [ext/sdl/build/libSDL2.a]"
    rm -rf ext/sdl/build
    cmake -S ext/sdl -B ext/sdl/build \
        -DCMAKE_SYSTEM_NAME=Linux \
        -DCMAKE_C_COMPILER=${TARGET}-gcc \
        -DCMAKE_BUILD_TYPE=Release \
        -DBUILD_SHARED_LIBS=OFF \
        -DSDL_LIBSAMPLERATE=OFF
        # -DBUILD_SHARED_LIBS=ON
        #        -DCMAKE_CXX_COMPILER=${TARGET}-g++ \
    cmake --build ext/sdl/build
    cp ext/sdl/build/libSDL2.a bin/libSDL2_x64.a
else
    echo "+> Found [bin/libSDL2_x64.a]"
fi

if [[ ! -f bin/libSDL2_image_x64.a ]]; then
    echo "-> Building [ext/sdl_image/build/libSDL2_image.a]"
    rm -rf ext/sdl_image/build
    cmake -S ext/sdl_image -B ext/sdl_image/build \
        -DCMAKE_SYSTEM_NAME=Linux \
        -DCMAKE_C_COMPILER=${TARGET}-gcc \
        -DCMAKE_BUILD_TYPE=Release \
        -DBUILD_SHARED_LIBS=OFF \
        -DSDL2_LIBRARY=ext/sdl/build/libSDL2.a \
        -DSDL2_MAIN_LIBRARY=ext/sdl/build/libSDL2main.a \
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
    cp ext/sdl_image/build/libSDL2_image.a bin/libSDL2_image_x64.a
else
    echo "+> Found [bin/libSDL2_image_x64.a]"
fi

if [[ ! -f bin/libSDL2_mixer_x64.a ]]; then
    echo "-> Building [ext/sdl_mixer/build/libSDL2_mixer.a]"
    rm -rf ext/sdl_mixer/build
    cmake -S ext/sdl_mixer -B ext/sdl_mixer/build \
        -DCMAKE_SYSTEM_NAME=Linux \
        -DCMAKE_C_COMPILER=${TARGET}-gcc \
        -DBUILD_SHARED_LIBS=OFF \
        -DSDL2_DIR=ext/sdl/build \
        -DSDL2_LIBRARY=ext/sdl/build/libSDL2.a \
        -DSDL2_INCLUDE_DIR=ext/sdl/include \
        -DSDL2MIXER_MOD=OFF \
        -DSDL2MIXER_MIDI_FLUIDSYNTH=OFF \
        -DSDL2MIXER_WAVPACK=OFF \
        -DSDL2MIXER_OPUS=OFF \
        -DSDL2MIXER_SAMPLES=OFF
    cmake --build ext/sdl_mixer/build
    cp ext/sdl_mixer/build/libSDL2_mixer.a bin/libSDL2_mixer_x64.a
else
    echo "+> Found [bin/libSDL2_mixer_x64.a]"
fi
