#!/usr/bin/env bash
set -euo pipefail

# TODO: Download source to ext directory
glew_url="https://github.com/nigels-com/glew/releases/download/glew-2.3.1/glew-2.3.1.zip"
sdl_url="https://github.com/libsdl-org/SDL/archive/refs/tags/release-2.32.8.zip"
sdl_image_url="https://github.com/libsdl-org/SDL_image/archive/refs/tags/release-2.8.10.zip"
sdl_mixer_url="https://github.com/libsdl-org/SDL_mixer/archive/refs/tags/release-2.8.1.zip"

# if no ext folder
mkdir -p ext
mkdir -p ext/glew ext/glew/src ext/glew/include
mkdir -p ext/sdl ext/sdl/src ext/sdl/include
mkdir -p ext/sdl_image ext/sdl_image/src ext/sdl_image/include
mkdir -p ext/sdl_mixer ext/sdl_mixer/src ext/sdl_mixer/include

# Glew
# https://github.com/nigels-com/glew/releases/download/glew-2.3.1/glew-2.3.1.zip
if [[ -f ext/glew.zip ]]; then
    echo "+ Found [glew.zip]"
else
    echo "-> Downloading [glew.zip]"
    curl -L ${glew_url} -o ext/glew.zip
fi
# curl -fs "${url}" -o inc/flecs/flecs.h

# SDL
if [[ -f ext/sdl.zip ]]; then
    echo "+ Found [ext/sdl.zip]"
else
    echo "-> Downloading [sdl.zip]"
    curl -L ${sdl_url} -o ext/sdl.zip
fi

# SDL Image
if [[ -f ext/sdl_image.zip ]]; then
    echo "+ Found [ext/sdl_image.zip]"
else
    echo "-> Downloading [sdl_image.zip]"
    curl -L ${sdl_image_url} -o ext/sdl_image.zip
fi

# SDL Mixer
if [[ -f ext/sdl_mixer.zip ]]; then
    echo "+ Found [ext/sdl_mixer.zip]"
else
    echo "-> Downloading [sdl_mixer.zip]"
    curl -L ${sdl_mixer_url} -o ext/sdl_mixer.zip
fi
