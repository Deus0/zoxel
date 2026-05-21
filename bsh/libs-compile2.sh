#!/usr/bin/env bash
set -euo pipefail

TARGET="${1:-linux}"
ARCH="${2:-x86_64}"

BIN_DIR="bin"
mkdir -p "$BIN_DIR"

SYSTEM_NAME=""
CC=""
SYSROOT=""
PKG_CONFIG_LIBDIR=""
SDL_LIB_NAME=""
SDL_IMAGE_LIB_NAME=""
SDL_MIXER_LIB_NAME=""

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

        SDL_LIB_NAME="SDL2.dll"
        SDL_IMAGE_LIB_NAME="SDL2_image.dll"
        SDL_MIXER_LIB_NAME="SDL2_mixer.dll"
        ;;

    linux)
        SYSTEM_NAME="Linux"

        case "$ARCH" in
            x86_64)
                CC="x86_64-linux-gnu-gcc"
                SYSROOT="/usr/x86_64-linux-gnu"
                PKG_CONFIG_LIBDIR="$SYSROOT/lib/pkgconfig:$SYSROOT/share/pkgconfig:$SYSROOT/usr/lib/pkgconfig"
                ;;
            arm64)
                CC="aarch64-linux-gnu-gcc"
                SYSROOT="/usr/aarch64-linux-gnu"
                PKG_CONFIG_LIBDIR="$SYSROOT/lib/pkgconfig:$SYSROOT/share/pkgconfig:$SYSROOT/usr/lib/pkgconfig"
                ;;
            *)
                echo "Unsupported Linux arch: $ARCH"
                exit 1
                ;;
        esac

        SDL_LIB_NAME="libSDL2.so"
        SDL_IMAGE_LIB_NAME="libSDL2_image.so"
        SDL_MIXER_LIB_NAME="libSDL2_mixer.so"
        ;;

    *)
        echo "Unknown target: $TARGET"
        echo "Usage: $0 [windows|linux] [x86_64|arm64]"
        exit 1
        ;;
esac

BUILD_SUFFIX="${TARGET}-${ARCH}"

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

# SDL2
build_if_missing \
    "SDL2" \
    "ext/sdl" \
    "ext/sdl/build-$BUILD_SUFFIX" \
    "$SDL_LIB_NAME" \
    "${SDL_LIB_NAME%.*}*.${SDL_LIB_NAME##*.}"

# SDL2_image
build_if_missing \
    "SDL2_image" \
    "ext/sdl_image" \
    "ext/sdl_image/build-$BUILD_SUFFIX" \
    "$SDL_IMAGE_LIB_NAME" \
    "${SDL_IMAGE_LIB_NAME%.*}*.${SDL_IMAGE_LIB_NAME##*.}" \
    -DSDL2_LIBRARY="ext/sdl/build-$BUILD_SUFFIX/$SDL_LIB_NAME" \
    -DSDL2_INCLUDE_DIR="ext/sdl/include" \
    -DSDL2IMAGE_PNG=ON \
    -DSDL2IMAGE_JPG=OFF \
    -DSDL2IMAGE_TIF=OFF \
    -DSDL2IMAGE_WEBP=OFF \
    -DSDL2IMAGE_AVIF=OFF \
    -DSDL2IMAGE_BMP=OFF \
    -DSDL2IMAGE_SAMPLES=OFF \
    -DSDL2IMAGE_TESTS=OFF

# SDL2_mixer
build_if_missing \
    "SDL2_mixer" \
    "ext/sdl_mixer" \
    "ext/sdl_mixer/build-$BUILD_SUFFIX" \
    "$SDL_MIXER_LIB_NAME" \
    "${SDL_MIXER_LIB_NAME%.*}*.${SDL_MIXER_LIB_NAME##*.}" \
    -DSDL2_LIBRARY="ext/sdl/build-$BUILD_SUFFIX/$SDL_LIB_NAME" \
    -DSDL2_INCLUDE_DIR="ext/sdl/include" \
    -DSDL2MIXER_MOD=OFF \
    -DSDL2MIXER_MIDI_FLUIDSYNTH=OFF \
    -DSDL2MIXER_WAVPACK=OFF \
    -DSDL2MIXER_OPUS=OFF \
    -DSDL2MIXER_BUILD_TESTS=OFF

echo "Build complete for target: $TARGET/$ARCH"
