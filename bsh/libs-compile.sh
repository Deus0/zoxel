#!/usr/bin/env bash
set -euo pipefail

# Required:
#   - SDL:              cmake
#   - Mesa:             meson python3-mako
#   - arm-to-x86_64     g++-x86-64-linux-gnu

source bsh/get-compiler.sh

TARGET="${1:-linux}"

# Detect host architecture.
HOST_ARCH="$(uname -m)"
case "$HOST_ARCH" in
    x86_64)
        HOST_ARCH="x64"
        ;;
    aarch64|arm64)
        HOST_ARCH="arm"
        ;;
    *)
        echo "Unsupported host architecture: $(uname -m)"
        exit 1
        ;;
esac

arc="$HOST_ARCH"


# Pass in args

# Architecture
[[ " $* " == *" --x64 "* ]] && arc="x64"
[[ " $* " == *" --arm "* ]] && arc="arm"

# Others
library="lib/${TARGET}_${arc}"
[[ " $* " == *" --docker "* ]] && library="${library}_docker"

echo "Host architecture: $HOST_ARCH"
echo "  Target architecture [${arc}]"
echo "  Target platform [$TARGET]"
echo "  Target [${library}]"
mkdir -p ${library}

USE_SDL3=1
USE_SDL_IMAGE=0
USE_SDL_MIXER=0
USE_MESA=0

for arg in "$@"; do
    case "$arg" in
        --sdl2)
            USE_SDL3=0
            ;;
        --sdl3)
            echo "+ Using SDL3"
            USE_SDL3=1
            ;;
        --sdl-image)
            USE_SDL_IMAGE=1
            ;;
        --sdl-mixer)
            USE_SDL_MIXER=1
            ;;
        --mesa)
            USE_MESA=1
            ;;
    esac
done

# BIN_DIR="bin"
# mkdir -p "$BIN_DIR"

SYSTEM_NAME=""
CC=""
CXX=""
SYSROOT=""
PKG_CONFIG_LIBDIR=""
SDL_LIB_NAME=""
SDL_LIB_IMPORT_NAME=""
SDL_IMAGE_LIB_NAME=""
SDL_MIXER_LIB_NAME=""
SDL_SRC_DIR=""

case "$TARGET" in
    windows)
        SYSTEM_NAME="Windows"

        case "${arc}" in
            x64) # x86_64)
                # CC="x86_64-w64-mingw32-gcc"
                CXX="x86_64-w64-mingw32-g++"
                ;;
            *)
                echo "Unsupported Windows arch: $ARCH"
                exit 1
                ;;
        esac

        if [[ "$USE_SDL3" -eq 1 ]]; then
            SDL_LIB_NAME="SDL3.dll"
            SDL_LIB_IMPORT_NAME="libSDL3.dll.a"
            SDL_IMAGE_LIB_NAME="SDL3_image.dll"
            SDL_MIXER_LIB_NAME="SDL3_mixer.dll"
        else
            SDL_LIB_NAME="SDL2.dll"
            SDL_LIB_IMPORT_NAME="libSDL2.dll.a"
            SDL_IMAGE_LIB_NAME="SDL2_image.dll"
            SDL_MIXER_LIB_NAME="SDL2_mixer.dll"
        fi
        ;;

    linux)
        SYSTEM_NAME="Linux"

        case "${arc}" in
            x64)
                # CC="x86_64-linux-gnu-gcc"
                if [[ "$HOST_ARCH" == "x64" ]]; then
                    # Native x64
                    CC="gcc"
                    CXX="g++"
                    SYSROOT=""
                    PKG_CONFIG_LIBDIR=""
                else
                    CC="x86_64-linux-gnu-gcc"
                    CXX="x86_64-linux-gnu-g++"
                    SYSROOT=""
                    PKG_CONFIG_LIBDIR="/usr/lib/x86_64-linux-gnu/pkgconfig"
                    # SYSROOT="/usr/x86_64-linux-gnu"
                    # PKG_CONFIG_LIBDIR="$SYSROOT/lib/pkgconfig:$SYSROOT/share/pkgconfig:$SYSROOT/usr/lib/pkgconfig"
                fi
                ;;

            arm) # arm64)
                # CC="aarch64-linux-gnu-gcc"

                if [[ "$HOST_ARCH" == "arm" ]]; then
                    SYSROOT=""
                    PKG_CONFIG_LIBDIR=""
                else
                    SYSROOT=""
                    PKG_CONFIG_LIBDIR="/usr/aarch64-linux-gnu/lib/pkgconfig:/usr/aarch64-linux-gnu/share/pkgconfig"
                    #SYSROOT="/usr/aarch64-linux-gnu"
                    #PKG_CONFIG_LIBDIR="$SYSROOT/lib/pkgconfig:$SYSROOT/share/pkgconfig:$SYSROOT/usr/lib/pkgconfig"
                fi
                ;;

            *)
                echo "Unsupported Linux arch: ${arc}"
                exit 1
                ;;
        esac

        if [[ "$USE_SDL3" -eq 1 ]]; then
            SDL_LIB_NAME="libSDL3.so"
            SDL_LIB_IMPORT_NAME="libSDL3.so"
            SDL_MIXER_LIB_NAME="libSDL3_mixer.so"
            SDL_IMAGE_LIB_NAME="libSDL3_image.so"
        else
            SDL_LIB_NAME="libSDL2.so"
            SDL_LIB_IMPORT_NAME="libSDL2.so"
            SDL_MIXER_LIB_NAME="libSDL2_mixer.so"
            SDL_IMAGE_LIB_NAME="libSDL2_image.so"
        fi
        ;;

    *)
        echo "Unknown target: $TARGET"
        echo "Usage: $0 [linux|windows] [x86_64|arm64] [--sdl3] [--sdl-image] [--sdl-mixer]"
        exit 1
        ;;
esac

CC="$(get_compiler "$HOST_ARCH" "$arc" "$TARGET")"

if [[ "$USE_SDL3" -eq 1 ]]; then
    SDL_SRC_DIR="ext/sdl3"
else
    SDL_SRC_DIR="ext/sdl2"
fi

BUILD_SUFFIX="${TARGET}-${arc}"

if [[ " $* " == *" --docker "* ]]; then
    BUILD_SUFFIX="${BUILD_SUFFIX}-docker"
fi

sdl_build_directory="$SDL_SRC_DIR/build-$BUILD_SUFFIX"

# mkdir -p ${sdl_build_directory}

echo "Compiler: $CC"
echo "  C++ $CXX"

if [[ -n "$SYSROOT" ]]; then
    echo "Sysroot: $SYSROOT"
else
    echo "Sysroot: native"
fi

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

    if [[ -f "$library/$OUTPUT_NAME" ]]; then
        echo "$library/$OUTPUT_NAME Found"
        return
    fi

    echo "-> Building $OUTPUT_NAME for $TARGET/${arc}"

    #if ! artifact_exists "$BUILD_DIR" "$OUTPUT_GLOB"; then
    #    echo "-> Building $OUTPUT_NAME for $TARGET/$ARCH"
    if [[ -n "$SYSROOT" ]]; then
        PKG_CONFIG_LIBDIR="$PKG_CONFIG_LIBDIR" \
        PKG_CONFIG_PATH="" \
        cmake -S "$SRC_DIR" -B "$BUILD_DIR" \
            -DCMAKE_SYSTEM_NAME="$SYSTEM_NAME" \
            -DCMAKE_C_COMPILER="$CC" \
            -DCMAKE_CXX_COMPILER="$CXX" \
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
            -DCMAKE_CXX_COMPILER="$CXX" \
            -DCMAKE_BUILD_TYPE=Release \
            "$@"
    fi

    cmake --build "$BUILD_DIR"

    local artifact=""
    artifact="$(locate_artifact "$BUILD_DIR" "$OUTPUT_GLOB")"

    if [[ -z "$artifact" ]]; then
        echo "Could not locate built artifact for $NAME in $BUILD_DIR"
        exit 1
    fi

    cp -L "$artifact" "$library/$OUTPUT_NAME"
    #if [[ -f "$artifact.0" ]]; then
    #    cp -L "$artifact.0" "bin/$OUTPUT_NAME.0"
    #    cp -L "$artifact.0" "$library/$OUTPUT_NAME.0"
    #else
    #    cp -L "$artifact" "bin/$OUTPUT_NAME"
    #    import_artifact="$(locate_artifact "$BUILD_DIR" "lib$OUTPUT_GLOB.a")"
    #    cp -L "$import_artifact" "$library/lib$OUTPUT_NAME.a"
    #fi
    if [[ -e "$artifact.0" ]]; then
        runtime_artifact="$artifact.0"
        runtime_name="$(basename "$runtime_artifact")"

        cp -L "$runtime_artifact" "bin/$runtime_name"
        cp -L "$runtime_artifact" "$library/$runtime_name"
    else
        cp -L "$artifact" "bin/$OUTPUT_NAME"

        import_artifact="$(locate_artifact "$BUILD_DIR" "lib$OUTPUT_GLOB.a")"
        cp -L "$import_artifact" "$library/lib$OUTPUT_NAME.a"
    fi
}

# SDL

build_if_missing \
    "SDL" \
    "$SDL_SRC_DIR" \
    "$SDL_SRC_DIR/build-$BUILD_SUFFIX" \
    "$SDL_LIB_NAME" \
    "${SDL_LIB_NAME%.*}*.${SDL_LIB_NAME##*.}" \
    -DSDL_X11_XTEST=OFF \
    -DSDL_TESTS=OFF \
    -DCMAKE_INSTALL_OLDINCLUDEDIR=

# SDL Mixer

if [[ "$USE_SDL_MIXER" -eq 1 ]]; then
    echo "Compiling SDL_Mixer with Import Library [$SDL_LIB_IMPORT_NAME]"
    if [[ "$USE_SDL3" -eq 1 ]]; then
        sdl3_lib="${library}/$SDL_LIB_NAME"
        echo "  - sdl3 [$sdl3_lib]"
        build_if_missing \
            "SDL3_mixer" \
            "ext/sdl3_mixer" \
            "ext/sdl3_mixer/build-$BUILD_SUFFIX" \
            "$SDL_MIXER_LIB_NAME" \
            "${SDL_MIXER_LIB_NAME%.*}*.${SDL_MIXER_LIB_NAME##*.}" \
            -DSDL3_DIR="$PWD/$SDL_SRC_DIR/build-$BUILD_SUFFIX" \
            -DCMAKE_INSTALL_OLDINCLUDEDIR= \
            -DSDLMIXER_MOD=OFF \
            -DSDLMIXER_MIDI_FLUIDSYNTH=OFF \
            -DSDLMIXER_WAVPACK=OFF \
            -DSDLMIXER_OPUS=OFF \
            -DSDLMIXER_SAMPLES=OFF \
            -DSDLMIXER_BUILD_TESTS=OFF \
            -DSDLMIXER_FLAC_LIBFLAC=OFF \
            -DSDLMIXER_VORBIS_VORBISFILE=OFF
    else
        sdl2_lib="${library}/$SDL_LIB_NAME"
        echo "  - sdl2 [$sdl2_lib]"
        build_if_missing \
            "SDL2_mixer" \
            "ext/sdl2_mixer" \
            "ext/sdl2_mixer/build-$BUILD_SUFFIX" \
            "$SDL_MIXER_LIB_NAME" \
            "${SDL_MIXER_LIB_NAME%.*}*.${SDL_MIXER_LIB_NAME##*.}" \
            -DCMAKE_DISABLE_FIND_PACKAGE_SDL2=TRUE \
            -DSDL2_LIBRARY="${sdl2_lib}" \
            -DSDL2_INCLUDE_DIR="$SDL_SRC_DIR/include" \
            -DSDL2MIXER_MOD=OFF \
            -DSDL2MIXER_MIDI_FLUIDSYNTH=OFF \
            -DSDL2MIXER_WAVPACK=OFF \
            -DSDL2MIXER_OPUS=OFF \
            -DSDL2MIXER_SAMPLES=OFF \
            -DSDL2MIXER_BUILD_TESTS=OFF
    fi
fi

# SDL Image

if [[ "$USE_SDL_IMAGE" -eq 1 ]]; then
    echo "Compiling SDL_Image"
    if [[ "$USE_SDL3" -eq 1 ]]; then
        SDL3_IMAGE_LIBRARY_ARG="-DSDL3_LIBRARY=$SDL_SRC_DIR/build-$BUILD_SUFFIX/$SDL_LIB_IMPORT_NAME"
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
        SDL2_IMAGE_LIBRARY_ARG="-DSDL2_LIBRARY=$SDL_SRC_DIR/build-$BUILD_SUFFIX/$SDL_LIB_IMPORT_NAME"
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

# Mesa

build_mesa_if_missing() {
    local SRC_DIR="$1"
    local BUILD_DIR="$2"
    local CROSS_FILE="$BUILD_DIR/cross.ini"

    if [[ -f "$BUILD_DIR/build.ninja" ]]; then
        echo "+ Found Mesa build [$BUILD_DIR]"
        return
    fi

    echo "-> Configuring Mesa for $TARGET/${arc}"

    mkdir -p "$BUILD_DIR"

    cat > "$CROSS_FILE" <<EOF
[binaries]
c = '$CC'
cpp = '$CXX'
ar = '${CC%-gcc}-ar'
strip = '${CC%-gcc}-strip'
pkg-config = 'pkg-config'

[host_machine]
system = 'linux'
cpu_family = 'x86_64'
cpu = 'x86_64'
endian = 'little'
EOF

    meson setup "$BUILD_DIR" "$SRC_DIR" \
        --cross-file "$CROSS_FILE" \
        --buildtype=release \
        -Dbuild-tests=false \
        -Dgallium-drivers=llvmpipe \
        -Dvulkan-drivers=[]

    meson compile -C "$BUILD_DIR"
}

if [[ "$USE_MESA" -eq 1 ]]; then
    if [[ "$TARGET" != "linux" ]]; then
        echo "Mesa is currently only enabled for Linux builds"
        exit 1
    fi
    mesa_build_directory="ext/mesa/build-$BUILD_SUFFIX"
    build_mesa_if_missing \
        "ext/mesa" \
        "$mesa_build_directory"
fi

echo "Libraries Build Completed [$TARGET/${arc}]"
