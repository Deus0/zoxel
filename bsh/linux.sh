#!/usr/bin/env bash
set -euo pipefail

# debug options
is_time_systems="0"
is_profiler="1"     # https://www.flecs.dev/explorer/?host=localhost
# bash inputs
game_name=$1    # zoxel
GLB=$2          # headless, opengl or vulkan
GFX=$3          # sdl, glut
ARC=$4          # x64
OS="linux"
ONARC=$(uname -m)
sdl_source="False"
sdl_images="False"
sdl_mixer="True"
package="False"
bin_filename="${game_name}" # -${GLB}-${GFX}-${ARC}
bin_path=bin/${bin_filename}.bin
compiler="gcc"
debug="False"
sources="src/main.c inc/flecs/flecs.c"
includes="-Iinc/flecs"
cflags="-fPIC -O3 -flto=auto -DNDEBUG"
dflags="-Dzox_game=${game_name} -Dflecssource -Dzox_linux"
libs="-lm -lpthread" # -Iinc

[[ " $* " == *" --debug "* ]] && debug="True"
[[ " $* " == *" --development "* ]] && debug="True"
[[ " $* " == *" --package "* ]] && package="True"

if [[ ${ARC} == "aarch64" ]]; then
    ARC="arm"
elif [[ ${ARC} == "x86_64" ]]; then
    ARC="x64"
fi

echo "Chosen Arc [${ARC}] - Running on [${ONARC}]"

if [[ ${ONARC} == "aarch64" && ${ARC} == "arm" ]]; then
    cflags+=" -march=native"
elif [[ ${ONARC} == "x86_64" && ${ARC} == "x64" ]]; then
    cflags+=" -march=native"
elif [[ ${ONARC} == "aarch64" && ${ARC} == "x64" ]]; then
    echo "Cross Compiler Set"
    echo "sudo apt install gcc-x86-64-linux-gnu"
    compiler="x86_64-linux-gnu-gcc"
    cflags+=" -march=x86-64"
    sdl_source="True"
    bsh/libs-download.sh
    bsh/libs-compile-linux-x64.sh
else
    echo "Running on Unsupported platform and target"
    exit
fi

if [[ ${debug} == "True" ]]; then
    echo "+ Added [debug]"
    cflags="-fPIC -Dzox_debug"
    cflags+=" -g3 -Wall -ggdb3"
    # cflags="-fPIC -O0 -g3 -Wall -ggdb3 -Dzox_debug"
    # cflags="-fPIC -O2 -g3 -Wall -ggdb3 -Dzox_debug"
    # cflags="-O2 -g -Dzox_debug"
    # cflags="-fPIC -g3 -Dzox_debug" #  -O0
    # For Regular Runs
    # cflags+=" -O3"
    # Memory Leaks
    cflags+=" -O0" #  -fsanitize=address"
    # For Full Debug
    # flags+=" -O0 -fno-omit-frame-pointer"
    #  -fsanitize=address
    bin_path="bin/${bin_filename}-dev.bin"
fi

if [[ ${is_profiler} == "1" ]]; then
    echo "+ Added zox_profiler"
    dflags+=" -Dzox_profiler"
fi

if [[ ${is_time_systems} == "1" ]]; then
    echo "+ Added [zox_time_systems]"
    dflags+=" -Dzox_time_systems"
fi

if [[ ${GLB} == "headless" ]]; then
    echo "+ Added [headless]"
    dflags+=" -Dzox_headless"
fi

if [[ ${GFX} == "sdl" ]]; then
    echo "+ Added [sdl]"
    dflags+=" -Dzox_sdl"
    if [[ ${sdl_images} == "True" ]]; then
        dflags+=" -Dzox_sdl_images"
    fi
    if [[ ${sdl_mixer} == "True" ]]; then
        dflags+=" -Dzox_sdl_mixer"
    fi
    if [[ ${sdl_source} == "True" ]]; then
        # libs+=" -Lext/sdl/build -Lext/sdl_image/build -Lext/sdl_mixer/build"
        libs+=" -static bin/libSDL2_x64.a bin/libSDL2_image_x64.a bin/libSDL2_mixer_x64.a"
        includes+=" -Iext/sdl/include -Iext/sdl_image/include -Iext/sdl_mixer/include"
        dflags+=" -Dsdlsource"
    else
        libs+=" -lSDL2"
        if [[ ${sdl_images} == "True" ]]; then
            libs+="  -lSDL2_image"
        fi
        if [[ ${sdl_mixer} == "True" ]]; then
            libs+=" -lSDL2_mixer"
        fi
    fi
fi

if [[ ${GLB} == "opengl" ]]; then
    echo "+ Added [opengl]"
    dflags+=" -Dzox_opengl"
    if [[ ${sdl_source} == "True" ]]; then
        echo "Cannot get Cross Compiler working with OpenGL [-lEGL -lGLESv2] yet.."
        exit
    else
        libs+=" -lEGL -lGLESv2"
    fi
fi

echo ""
echo "Building [${bin_path}]"
echo "  - Compiler [${compiler}]"
echo "  - CFlags [${cflags}]"
echo "  - DFlags [${dflags}]"
echo "  - Libs [${libs}]"
echo "  - Includes [${includes}]"
echo ""

mkdir -p bin
${compiler} ${cflags} ${sources} -o "${bin_path}" ${includes} ${dflags} ${libs}
echo "+ Completed Build [${bin_path}]"

# ---- Packaging ----
if [[ ${package} == "True" ]]; then
    date_str=$(date +%Y_%m_%d)
    zip_name="bin/${game_name}_${OS}_${ARC}_${GLB}_${GFX}_${date_str}.zip"
    echo ""
    echo "> Packaging [${zip_name}]"
    rm -f ${zip_name}
    zip -q -r "${zip_name}" res
    zip -j "${zip_name}" "${bin_path}"
    echo "+ Created package [${zip_name}]"
fi
