#!/usr/bin/env bash
set -euo pipefail

USE_SDL_IMAGE=0
USE_SDL_MIXER=0
for arg in "$@"; do
    case "$arg" in
        --sdl-image)
            USE_SDL_IMAGE=1
            ;;
        --sdl-mixer)
            USE_SDL_MIXER=1
            ;;
    esac
done

# Compile SDL first:
if [[ ! -f bin/SDL2.dll ]]; then
    echo "-> Building for SDL2.dll"
    rm -rf ext/sdl/build
    cmake -S ext/sdl -B ext/sdl/build \
        -DCMAKE_SYSTEM_NAME=Windows \
        -DCMAKE_C_COMPILER=i686-w64-mingw32-gcc \
        -DCMAKE_BUILD_TYPE=Release
    cmake --build ext/sdl/build
    cp ext/sdl/build/SDL2.dll bin/SDL2.dll
else
    echo "SDL2.dll Found"
fi

if [ "$USE_SDL_IMAGE" -eq 1 ]; then
    if [[ ! -f bin/SDL2_image.dll ]]; then
        echo "-> Building for SDL2_image.dll"
        rm -rf ext/sdl_image/build
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
        cp ext/sdl_image/build/SDL2_image.dll bin/SDL2_image.dll
    else
        echo "SDL2_image.dll Found"
    fi
fi

if [ "$USE_SDL_MIXER" -eq 1 ]; then
    if [[ ! -f bin/SDL2_mixer.dll ]]; then
        echo "-> Building for SDL2_mixer.dll"
        rm -rf ext/sdl_mixer/build
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
            -DSDL2MIXER_SAMPLES=OFF
        cmake --build ext/sdl_mixer/build
        cp ext/sdl_mixer/build/SDL2_mixer.dll bin/SDL2_mixer.dll
    else
        echo "SDL2_mixer.dll Found"
    fi
fi