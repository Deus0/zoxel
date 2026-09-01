#!/usr/bin/env bash
set -euo pipefail

glew_url="https://github.com/nigels-com/glew/releases/download/glew-2.3.1/glew-2.3.1.zip"
sdl3_url="https://github.com/libsdl-org/SDL/archive/refs/tags/release-3.4.14.zip"
sdl_url="https://github.com/libsdl-org/SDL/archive/refs/tags/release-2.32.8.zip"
sdl_image_url="https://github.com/libsdl-org/SDL_image/archive/refs/tags/release-2.8.10.zip"
sdl_mixer_url="https://github.com/libsdl-org/SDL_mixer/archive/refs/tags/release-2.8.1.zip"
sdl3_mixer_url="https://github.com/libsdl-org/SDL_mixer/archive/refs/tags/release-3.2.4.zip"
# as khadas as mason 1.3.2 only
# mesa_url="https://archive.mesa3d.org/mesa-26.1.8.tar.xz"
mesa_url="https://archive.mesa3d.org/mesa-25.1.9.tar.xz"

sdl_filepath="ext/sdl2.zip"
sdl3_filepath="ext/sdl3.zip"
mesa_filepath="ext/mesa.tar.xz"

USE_SDL3=1
USE_GLEW=0
USE_SDL_IMAGE=0
USE_SDL_MIXER=0
USE_MESA=0

for arg in "$@"; do
    case "$arg" in
        --sdl2)
            USE_SDL3=0
            ;;
        --sdl3)
            USE_SDL3=1
            ;;
        --glew)
            USE_GLEW=1
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

echo "Downloading Libraries: SDL3 [$USE_SDL3], Glew [$USE_GLEW], Image [$USE_SDL_IMAGE], Mixer [$USE_SDL_MIXER]"
echo "  - Mesa [$USE_MESA]"

mkdir -p ext

download_and_extract() {
    local url="$1"
    local zip="$2"
    local dir="$3"

    if [[ -f "$zip" ]]; then
        echo "+ Found [$zip]"
    else
        echo "-> Downloading [$zip] from [$url]"
        curl -L "$url" -o "$zip"
    fi

    if [[ ! -f "$dir/CMakeLists.txt" ]]; then
        echo "-> Extracting [$zip] to [$dir]"

        mkdir -p "$dir"

        local temp_dir
        temp_dir="$(mktemp -d)"

        unzip -q "$zip" -d "$temp_dir"

        local source_dir
        source_dir="$(find "$temp_dir" -mindepth 1 -maxdepth 1 -type d | head -n 1)"

        if [[ -z "$source_dir" ]]; then
            echo "Could not find source directory in $zip"
            rm -rf "$temp_dir"
            exit 1
        fi

        cp -a "$source_dir"/. "$dir"/

        rm -rf "$temp_dir"
    else
        echo "+ Found extracted source [$dir]"
    fi
}

download_and_extract_tar() {
    local url="$1"
    local archive="$2"
    local dir="$3"
    if [[ -f "$archive" ]]; then
        echo "+ Found [$archive]"
    else
        echo "-> Downloading [$archive] from [$url]"
        curl -L "$url" -o "$archive"
    fi
    if [[ ! -f "$dir/meson.build" ]]; then
        echo "-> Extracting [$archive] to [$dir]"
        mkdir -p "$dir"
        local temp_dir
        temp_dir="$(mktemp -d)"
        tar -xf "$archive" -C "$temp_dir"
        local source_dir
        source_dir="$(find "$temp_dir" -mindepth 1 -maxdepth 1 -type d | head -n 1)"
        if [[ -z "$source_dir" ]]; then
            echo "Could not find source directory in $archive"
            rm -rf "$temp_dir"
            exit 1
        fi
        cp -a "$source_dir"/. "$dir"/
        rm -rf "$temp_dir"
    else
        echo "+ Found extracted source [$dir]"
    fi
}

# GLEW

if [[ "$USE_GLEW" -eq 1 ]]; then
    download_and_extract \
        "$glew_url" \
        "ext/glew.zip" \
        "ext/glew"
fi

# SDL

if [[ "$USE_SDL3" -eq 1 ]]; then
    download_and_extract \
        "$sdl3_url" \
        "$sdl3_filepath" \
        "ext/sdl3"
else
    download_and_extract \
        "$sdl_url" \
        "$sdl_filepath" \
        "ext/sdl2"
fi

# SDL Mixer

if [[ "$USE_SDL_MIXER" -eq 1 ]]; then
    if [[ "$USE_SDL3" -eq 1 ]]; then
        download_and_extract \
            "$sdl3_mixer_url" \
            "ext/sdl3_mixer.zip" \
            "ext/sdl3_mixer"
    else
        download_and_extract \
            "$sdl_mixer_url" \
            "ext/sdl2_mixer.zip" \
            "ext/sdl2_mixer"
    fi
fi

# SDL Image

if [[ "$USE_SDL_IMAGE" -eq 1 ]]; then
    download_and_extract \
        "$sdl_image_url" \
        "ext/sdl2_image.zip" \
        "ext/sdl2_image"
fi

# Mesa

if [[ "$USE_MESA" -eq 1 ]]; then
    download_and_extract_tar \
        "$mesa_url" \
        "$mesa_filepath" \
        "ext/mesa"
fi
