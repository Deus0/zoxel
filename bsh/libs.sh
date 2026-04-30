#!/usr/bin/env bash
set -euo pipefail

# TODO: Download source to ext directory

# if no ext folder
mkdir -p ext
mkdir -p ext/glew ext/glew/src ext/glew/include
mkdir -p ext/sdl ext/sdl/src ext/sdl/include
mkdir -p ext/sdl_image ext/sdl_image/src ext/sdl_image/include
mkdir -p ext/sdl_mixer ext/sdl_mixer/src ext/sdl_mixer/include

# Glew
# https://github.com/nigels-com/glew/releases/download/glew-2.3.1/glew-2.3.1.zip

# SDL
# https://github.com/libsdl-org/SDL/archive/refs/tags/release-2.32.8.zip

# SDL Image
# https://github.com/libsdl-org/SDL_image/archive/refs/tags/release-2.8.10.zip

# SDL Mixer
# https://github.com/libsdl-org/SDL_mixer/archive/refs/tags/release-2.8.1.zip
