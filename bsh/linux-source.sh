#!/usr/bin/env bash
set -euo pipefail

echo "Building Zoxel Linux [Source]"

./bsh/libs.sh

#  ext/sdl_mixer/src
src_sdl=$(find ext/sdl/src ext/sdl_image/src -name '*.c')

echo "src_sdl:\n $src_sdl"

gcc \
-fPIC \
-O3 \
-march=native \
-flto=auto \
-DNDEBUG \
\
inc/flecs/flecs.c \
$src_sdl \
src/main.c \
\
-o bin/zoxel \
\
-lm \
-lpthread \
-lEGL \
-lGLESv2 \
-Iinc \
\
-Dzox_debug \
-Dzox_game=zoxel \
-Dzox_linux \
-Dflecssource \
-Dsdlsource \
-Dzox_sdl \
-Dzox_sdl_images \
\
-Iext/sdl/include \
-Iext/sdl/src \
-Iext/sdl_image/include \
-D_REENTRANT \
-D_GNU_SOURCE \
-DSDL_VIDEO_DRIVER_WAYLAND=1

#-DSDL_USE_LIBDBUS=0 \
# -DSDL_DISABLE_DBUS=1 \
# -DHAVE_STDIO_H=1 \
# -DHAVE_STDLIB_H=1 \
# -DHAVE_STRING_H=1 \
# -DSDL_THREADS_DISABLED \
# -DSDL_USE_LIBDBUS=0 \
# -DSDL_DISABLE_DBUS=1


# -Dzox_sdl_mixer \
# -Iext/sdl_mixer/include \
# -Iext/sdl_mixer/src \
# -Iext/sdl_mixer/src/codecs \
