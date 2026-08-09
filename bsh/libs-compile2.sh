#!/usr/bin/env bash
set -euo pipefail

TARGET="${1:-linux}"
ARCH="${2:-x86_64}"

echo "Compiling Libraries [$TARGET]:[$ARCH]"

USE_SDL3=0
USE_SDL_IMAGE=0
USE_SDL_MIXER=0

for arg in "$@"; do
    case "$arg" in
        --sdl3)
            USE_SDL3=1
            ;;
        --sdl-image)
            USE_SDL_IMAGE=1
            ;;
        --sdl-mixer)
            USE_SDL_MIXER=1
            ;;
    esac
done

BIN_DIR="bin"
mkdir -p "$BIN_DIR"

SYSTEM_NAME=""
CC=""
SYSROOT=""
PKG_CONFIG_LIBDIR=""

SDL_LIB_NAME=""
SDL_IMAGE_LIB_NAME=""
SDL_MIXER_LIB_NAME=""

SDL_SRC_DIR=""

# Detect host architecture.
HOST_ARCH="$(uname -m)"

case "$HOST_ARCH" in
    x86_64)
        HOST_ARCH="x86_64"
        ;;
    aarch64|arm64)
        HOST_ARCH="arm64"
        ;;
    *)
        echo "Unsupported host architecture: $(uname -m)"
        exit 1
        ;;
esac

echo "Host architecture: $HOST_ARCH"

case "$TARGET" in
    windows)
        SYSTEM_NAME="Windows"

        case "$ARCH" in
            x86_64)
                CC="x86_64-w64-mingw32-gcc"
                ;;
            arm64)
                CC="aarch64-w64-mingw32-gcc"
                ;;
            *)
                echo "Unsupported Windows arch: $ARCH"
                exit 1
                ;;
        esac

        if [[ "$USE_SDL3" -eq 1 ]]; then
            SDL_LIB_NAME="SDL3.dll"
            SDL_IMAGE_LIB_NAME="SDL3_image.dll"
            SDL_MIXER_LIB_NAME="SDL3_mixer.dll"
        else
            SDL_LIB_NAME="SDL2.dll"
            SDL_IMAGE_LIB_NAME="SDL2_image.dll"
            SDL_MIXER_LIB_NAME="SDL2_mixer.dll"
        fi
        ;;

    linux)
        SYSTEM_NAME="Linux"

        case "$ARCH" in
            x86_64)
                CC="x86_64-linux-gnu-gcc"

                if [[ "$HOST_ARCH" == "x86_64" ]]; then
                    SYSROOT=""
                    PKG_CONFIG_LIBDIR=""
                else
                    SYSROOT="/usr/x86_64-linux-gnu"
                    PKG_CONFIG_LIBDIR="$SYSROOT/lib/pkgconfig:$SYSROOT/share/pkgconfig:$SYSROOT/usr/lib/pkgconfig"
                fi
                ;;

            arm64)
                CC="aarch64-linux-gnu-gcc"

                if [[ "$HOST_ARCH" == "arm64" ]]; then
                    SYSROOT=""
                    PKG_CONFIG_LIBDIR=""
                else
                    SYSROOT="/usr/aarch64-linux-gnu"
                    PKG_CONFIG_LIBDIR="$SYSROOT/lib/pkgconfig:$SYSROOT/share/pkgconfig:$SYSROOT/usr/lib/pkgconfig"
                fi
                ;;

            *)
                echo "Unsupported Linux arch: $ARCH"
                exit 1
                ;;
        esac

        if [[ "$USE_SDL3" -eq 1 ]]; then
            SDL_LIB_NAME="libSDL3.so"
            SDL_IMAGE_LIB_NAME="libSDL3_image.so"
            SDL_MIXER_LIB_NAME="libSDL3_mixer.so"
        else
            SDL_LIB_NAME="libSDL2.so"
            SDL_IMAGE_LIB_NAME="libSDL2_image.so"
            SDL_MIXER_LIB_NAME="libSDL2_mixer.so"
        fi
        ;;

    *)
        echo "Unknown target: $TARGET"
        echo "Usage: $0 [linux|windows] [x86_64|arm64] [--sdl3] [--sdl-image] [--sdl-mixer]"
        exit 1
        ;;
esac

if [[ "$USE_SDL3" -eq 1 ]]; then
    SDL_SRC_DIR="ext/sdl3"
else
    SDL_SRC_DIR="ext/sdl2"
fi

BUILD_SUFFIX="${TARGET}-${ARCH}"

echo "Compiler: $CC"

if [[ -n "$SYSROOT" ]]; then
    echo "Sysroot: $SYSROOT"
else
    echo "Sysroot: native"
fi

artifact_exists() {
    local dir="$1"
    local pattern="$2"

    compgen -G "$dir/$pattern" > /dev/null
}

locate_artifact() {
    local dir="$1"
    local pattern="$2"
    local found=""

    found="$(find "$dir" -type f -name "$pattern" | sort | head -n 1 || true)"

    if [[ -n "$found" ]]; then
        printf '%s\n' "$found"
        return 0
    fi

    found="$(find "$dir" -type l -name "$pattern" | sort | head -n 1 || true)"

    if [[ -n "$found" ]]; then
        printf '%s\n' "$found"
        return 0
    fi

    return 1
}

build_if_missing() {
    local NAME="$1"
    local SRC_DIR="$2"
    local BUILD_DIR="$3"
    local OUTPUT_NAME="$4"
    local OUTPUT_GLOB="$5"

    shift 5

    if ! artifact_exists "$BUILD_DIR" "$OUTPUT_GLOB"; then
        echo "-> Building $OUTPUT_NAME for $TARGET/$ARCH"

        if [[ -n "$SYSROOT" ]]; then
            PKG_CONFIG_LIBDIR="$PKG_CONFIG_LIBDIR" \
            PKG_CONFIG_PATH="" \
            cmake -S "$SRC_DIR" -B "$BUILD_DIR" \
                -DCMAKE_SYSTEM_NAME="$SYSTEM_NAME" \
                -DCMAKE_C_COMPILER="$CC" \
                -DCMAKE_BUILD_TYPE=Release \
                -DCMAKE_SYSROOT="$SYSROOT" \
                -DCMAKE_FIND_ROOT_PATH="$SYSROOT" \
                -DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=NEVER \
                -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY \
                -DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY \
                -DCMAKE_FIND_ROOT_PATH_MODE_PACKAGE=ONLY \
                "$@"
        else
            cmake -S "$SRC_DIR" -B "$BUILD_DIR" \
                -DCMAKE_SYSTEM_NAME="$SYSTEM_NAME" \
                -DCMAKE_C_COMPILER="$CC" \
                -DCMAKE_BUILD_TYPE=Release \
                "$@"
        fi

        cmake --build "$BUILD_DIR"
    else
        echo "$OUTPUT_NAME Found"
    fi

    local artifact=""
    artifact="$(locate_artifact "$BUILD_DIR" "$OUTPUT_GLOB")"

    if [[ -z "$artifact" ]]; then
        echo "Could not locate built artifact for $NAME in $BUILD_DIR"
        exit 1
    fi

    cp -L "$artifact" "$BIN_DIR/$OUTPUT_NAME"
}

# SDL

build_if_missing \
    "SDL" \
    "$SDL_SRC_DIR" \
    "$SDL_SRC_DIR/build-$BUILD_SUFFIX" \
    "$SDL_LIB_NAME" \
    "${SDL_LIB_NAME%.*}*.${SDL_LIB_NAME##*.}"

# SDL Image

if [[ "$USE_SDL_IMAGE" -eq 1 ]]; then
    if [[ "$USE_SDL3" -eq 1 ]]; then
        SDL3_IMAGE_LIBRARY_ARG="-DSDL3_LIBRARY=$SDL_SRC_DIR/build-$BUILD_SUFFIX/$SDL_LIB_NAME"
        SDL3_IMAGE_INCLUDE_ARG="-DSDL3_INCLUDE_DIR=$SDL_SRC_DIR/include"

        build_if_missing \
            "SDL3_image" \
            "ext/sdl_image" \
            "ext/sdl_image/build-$BUILD_SUFFIX" \
            "$SDL_IMAGE_LIB_NAME" \
            "${SDL_IMAGE_LIB_NAME%.*}*.${SDL_IMAGE_LIB_NAME##*.}" \
            "$SDL3_IMAGE_LIBRARY_ARG" \
            "$SDL3_IMAGE_INCLUDE_ARG" \
            -DSDL3IMAGE_PNG=ON \
            -DSDL3IMAGE_JPG=OFF \
            -DSDL3IMAGE_TIF=OFF \
            -DSDL3IMAGE_WEBP=OFF \
            -DSDL3IMAGE_AVIF=OFF \
            -DSDL3IMAGE_BMP=OFF \
            -DSDL3IMAGE_SAMPLES=OFF \
            -DSDL3IMAGE_TESTS=OFF
    else
        SDL2_IMAGE_LIBRARY_ARG="-DSDL2_LIBRARY=$SDL_SRC_DIR/build-$BUILD_SUFFIX/$SDL_LIB_NAME"
        SDL2_IMAGE_INCLUDE_ARG="-DSDL2_INCLUDE_DIR=$SDL_SRC_DIR/include"

        build_if_missing \
            "SDL2_image" \
            "ext/sdl_image" \
            "ext/sdl_image/build-$BUILD_SUFFIX" \
            "$SDL_IMAGE_LIB_NAME" \
            "${SDL_IMAGE_LIB_NAME%.*}*.${SDL_IMAGE_LIB_NAME##*.}" \
            "$SDL2_IMAGE_LIBRARY_ARG" \
            "$SDL2_IMAGE_INCLUDE_ARG" \
            -DSDL2IMAGE_PNG=ON \
            -DSDL2IMAGE_JPG=OFF \
            -DSDL2IMAGE_TIF=OFF \
            -DSDL2IMAGE_WEBP=OFF \
            -DSDL2IMAGE_AVIF=OFF \
            -DSDL2IMAGE_BMP=OFF \
            -DSDL2IMAGE_SAMPLES=OFF \
            -DSDL2IMAGE_TESTS=OFF
    fi
fi

# SDL Mixer

if [[ "$USE_SDL_MIXER" -eq 1 ]]; then
    if [[ "$USE_SDL3" -eq 1 ]]; then
        SDL3_MIXER_LIBRARY_ARG="-DSDL3_LIBRARY=$SDL_SRC_DIR/build-$BUILD_SUFFIX/$SDL_LIB_NAME"
        SDL3_MIXER_INCLUDE_ARG="-DSDL3_INCLUDE_DIR=$SDL_SRC_DIR/include"

        build_if_missing \
            "SDL3_mixer" \
            "ext/sdl_mixer" \
            "ext/sdl_mixer/build-$BUILD_SUFFIX" \
            "$SDL_MIXER_LIB_NAME" \
            "${SDL_MIXER_LIB_NAME%.*}*.${SDL_MIXER_LIB_NAME##*.}" \
            "$SDL3_MIXER_LIBRARY_ARG" \
            "$SDL3_MIXER_INCLUDE_ARG" \
            -DSDL3MIXER_MOD=OFF \
            -DSDL3MIXER_MIDI_FLUIDSYNTH=OFF \
            -DSDL3MIXER_WAVPACK=OFF \
            -DSDL3MIXER_OPUS=OFF \
            -DSDL3MIXER_SAMPLES=OFF \
            -DSDL3MIXER_BUILD_TESTS=OFF
    else
        SDL2_MIXER_LIBRARY_ARG="-DSDL2_LIBRARY=$SDL_SRC_DIR/build-$BUILD_SUFFIX/$SDL_LIB_NAME"
        SDL2_MIXER_INCLUDE_ARG="-DSDL2_INCLUDE_DIR=$SDL_SRC_DIR/include"

        build_if_missing \
            "SDL2_mixer" \
            "ext/sdl_mixer" \
            "ext/sdl_mixer/build-$BUILD_SUFFIX" \
            "$SDL_MIXER_LIB_NAME" \
            "${SDL_MIXER_LIB_NAME%.*}*.${SDL_MIXER_LIB_NAME##*.}" \
            "$SDL2_MIXER_LIBRARY_ARG" \
            "$SDL2_MIXER_INCLUDE_ARG" \
            -DSDL2MIXER_MOD=OFF \
            -DSDL2MIXER_MIDI_FLUIDSYNTH=OFF \
            -DSDL2MIXER_WAVPACK=OFF \
            -DSDL2MIXER_OPUS=OFF \
            -DSDL2MIXER_SAMPLES=OFF \
            -DSDL2MIXER_BUILD_TESTS=OFF
    fi
fi

echo "Build complete for target: $TARGET/$ARCH"