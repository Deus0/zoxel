#!/usr/bin/env bash
set -euo pipefail

# debug options
is_profiler="0"         # https://www.flecs.dev/explorer/?host=localhost
is_safety_checks="0"    # lets stay safe for now
is_time_systems="0"
is_fast_dev="0"         # -O3
# settings
is_desktop_gl="1"       # Use GL libs instead of EGL on desktop
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
bin_filename="${game_name}" # -${GLB}-${GFX}-${ARC}
bin_path=bin/${bin_filename}.bin
compiler="gcc"
sources="src/main.c inc/flecs/flecs.c"
includes="-Iinc/flecs"
cflags="-std=gnu99 -fPIC"
dflags="-Dzox_game=${game_name} -Dflecssource -Dzox_linux"
libs="-lm -lpthread" # -Iinc

debug="False"
[[ " $* " == *" --debug "* ]] && debug="True"
[[ " $* " == *" --development "* ]] && debug="True"

package="False"
[[ " $* " == *" --package "* ]] && package="True"

is_sdl3="False"
[[ " $* " == *" --sdl3 "* ]] && is_sdl3="True"

is_static="False"
[[ " $* " == *" --static "* ]] && is_static="True"

if [[ ${ARC} == "aarch64" ]]; then
    ARC="arm"
elif [[ ${ARC} == "x86_64" ]]; then
    ARC="x64"
fi

echo "Chosen Arc [${ARC}] - Running on [${ONARC}]"

# Our  Libs
if [[ ${is_static} == "True" ]]; then
    sdl_mixer="False"
    bsh/libs-download.sh --sdl3 # --sdl-mixer
    bsh/libs-compile2.sh linux x86_64 --sdl3 # --sdl-mixer
fi

if [[ ${ONARC} == "aarch64" && ${ARC} == "arm" ]]; then
    cflags+=" -march=native"
    is_desktop_gl="0"
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
    bin_path="bin/${bin_filename}-dev.bin"
    # cflags="-O2 -g -Dzox_debug"
    # cflags="-fPIC -g3 -Dzox_debug" #  -O0
    # For Regular Runs
    if [[ ${is_fast_dev} == "1" ]]; then
        cflags+=" -O3 -g"
    else
        dflags+=" -Dzox_debug"
        cflags+=" -Wall -ggdb3"
        cflags+=" -O0 -g3"
    fi
    # Memory Leaks Full Debug
    # cflags+=" -fno-omit-frame-pointer""
    # cflags+=" -fsanitize=address"
else
    # Release Builds
    cflags+=" -O3 -flto=auto -DNDEBUG"
fi

if [[ ${is_profiler} == "1" ]]; then
    echo "+ Added zox_profiler"
    dflags+=" -Dzox_profiler"
fi

if [[ ${is_safety_checks} == "1" ]]; then
    echo "+ Added zox_safety_checks"
    dflags+=" -Dzox_safety_checks"
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
    if [[ ${is_sdl3} == "True" ]]; then
        echo "+ Added [sdl3]"
        dflags+=" -Dzox_sdl3"
        includes+=" -Iext/sdl3/include"
        libs+=" -Lbin -lSDL3 -Wl,-rpath,'\$ORIGIN'"
     elif [[ ${sdl_source} == "True" ]]; then
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
        if [[ ${is_desktop_gl} == "1" ]]; then
            libs+=" -lGL"
        else
            libs+=" -lEGL -lGLESv2"
        fi
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
