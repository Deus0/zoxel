#!/usr/bin/env bash
set -euo pipefail

# NOTE: Converts the shell flags into flags for our game code

# debug options
OS="linux"
is_safety_checks="1"    # lets stay safe for now
is_profiler="0"         # https://www.flecs.dev/explorer/?host=localhost
is_time_systems="0"
is_fast_dev="0"         # -O3
# settings
is_desktop_gl="1"       # Use GL libs instead of EGL on desktop
# bash inputs

ONARC=$(uname -m)
if [[ ${ONARC} == "aarch64" ]]; then
    ONARC="arm"
elif [[ ${ONARC} == "x86_64" ]]; then
    ONARC="x64"
fi
ARC="${ONARC}"         # x64 or arm

sdl_source="0"
sdl_images="0"
sdl_mixer="1"

# BUILD ARGS #
game_name="zoxel"
debug="0"
logs="0"
verbose="0"
package="0"
is_static="1"
GLB="opengl"      # headless, opengl or vulkan
GFX="sdl"         # sdl, glut
is_sdl3="1"       # sdl2, sdl3
server="0"

if [[ $# -gt 0 && ${1} != --* ]]; then
    game_name="$1"
fi

[[ " $* " == *" --nomixer "* ]] && sdl_mixer="0"
[[ " $* " == *" --debug "* ]] && debug="1"
[[ " $* " == *" --release "* ]] && debug="0"
[[ " $* " == *" --nologs "* ]] && logs="0"
[[ " $* " == *" --logs "* ]] && logs="1"
[[ " $* " == *" --timings "* ]] && is_time_systems="1"
[[ " $* " == *" --profile "* ]] && is_profiler="1"
[[ " $* " == *" --verbose "* ]] && verbose="1"
[[ " $* " == *" --package "* ]] && package="1"
[[ " $* " == *" --system "* ]] && is_static="0"
[[ " $* " == *" --static "* ]] && is_static="1"
[[ " $* " == *" --server "* ]] && server="1"
# Architecture
[[ " $* " == *" --x64 "* ]] && ARC="x64"
[[ " $* " == *" --arm "* ]] && ARC="arm"
# GFX
[[ " $* " == *" --headless "* ]] && GFX="headless"
[[ " $* " == *" --sdl "* ]] && GFX="sdl"
[[ " $* " == *" --glut "* ]] && GFX="glut"
[[ " $* " == *" --sdl2 "* ]] && is_sdl3="0"
[[ " $* " == *" --sdl3 "* ]] && is_sdl3="1"
# Graphics Library
[[ " $* " == *" --headless "* ]] && GLB="headless"
[[ " $* " == *" --opengl "* ]] && GLB="opengl"
[[ " $* " == *" --vulkan "* ]] && GLB="vulkan"

bin_filename="${game_name}"
output_extension="bin"
output_folder="bin"
bin_path="${output_folder}/${bin_filename}.${output_extension}"
compiler="gcc"
sources="src/main.c inc/flecs/flecs.c"
includes="-Iinc/flecs"
cflags="-std=gnu99 -fPIC"
dflags="-Dzox_game=${game_name} -Dflecssource -Dzox_linux"
libs="-lm -lpthread -ldl"
package_path="zip"
# architecture="x86_64"   # base arch name
library="lib/${OS}_${ARC}"

echo "Chosen Arc [${ARC}] - Running on [${ONARC}]"

# Special output name for headless
[[ "${GLB}" == "headless" ]] && bin_filename="${bin_filename}-headless"
[[ "${debug}" == "1" ]] && bin_filename="${bin_filename}-dev"

bin_path="bin/${bin_filename}.${output_extension}"

# used to reduce instructions to a single CPU
#  cflags+=" -march=native"

# Our  Libs
if [[ "${GFX}" == "sdl" && ${is_static} == "1" ]]; then
    # Make use local lib files during runtime
    libs+=" -Wl,-rpath,\$ORIGIN"
    echo ""
    echo "--------------------------------------------"
    lib_args=""
    [[ ${is_sdl3} == "1" ]] && lib_args+=" --sdl3"
    [[ ${sdl_mixer} == "1" ]] && lib_args+=" --sdl-mixer"
    bsh/libs-download.sh ${lib_args}
    echo "--------------------------------------------"
    bsh/libs-compile.sh linux ${ARC} ${lib_args}
    echo "--------------------------------------------"
    echo ""
fi

if [[ ${ARC} == "arm" ]]; then
    echo "+ [Arm] Enabled [gles]"
    is_desktop_gl="0"
fi

if [[ ${debug} == "1" ]]; then
    echo "+ Added [debug]"
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
    echo "+ Added [flecs_profiler]"
    dflags+=" -Dflecs_profiler"
fi

if [[ ${is_safety_checks} == "1" ]]; then
    echo "+ Added [zox_safety_checks]"
    dflags+=" -Dzox_safety_checks"
fi

if [[ ${logs} == "1" ]]; then
    echo "+ Added [zox_logs]"
    dflags+=" -Dzox_logs"
fi

if [[ ${is_time_systems} == "1" ]]; then
    echo "+ Added [zox_time_systems]"
    dflags+=" -Dzox_time_systems"
fi

if [[ ${verbose} == "1"  ]]; then
    echo "+ Added [zox_verbose]"
    dflags+=" -Dzox_verbose"
fi

if [[ ${GLB} == "headless" ]]; then
    echo "+ Added [zox_headless]"
    dflags+=" -Dzox_headless"
fi

if [[ ${server} == "1" ]]; then
    echo "+ Added [zox_server]"
    dflags+=" -Dzox_server"
fi

if [[ ${GLB} == "opengl" ]]; then
    echo "+ Added [zox_opengl]"
    dflags+=" -Dzox_opengl"
    if [[ ${sdl_source} == "1" ]]; then
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

if [[ ${GFX} == "sdl" ]]; then
    echo "+ Added [zox_sdl]"
    dflags+=" -Dzox_sdl"
    libs+=" -L${library}" # static libs for build
    if [[ ${sdl_images} == "1" ]]; then
        dflags+=" -Dzox_sdl_images"
    fi
    if [[ ${sdl_mixer} == "1" ]]; then
        dflags+=" -Dzox_sdl_mixer"
    fi
    if [[ ${is_sdl3} == "1" ]]; then
        echo "+ Added [zox_sdl3]"
        dflags+=" -Dzox_sdl3"
        # for local / static
        if [[ ${is_static} == "0" ]]; then
            libs+=" -lSDL3"
            if [[ ${sdl_mixer} == "1" ]]; then
                libs+=" -lSDL3_mixer"
            fi
        else
            libs+=" ${library}/libSDL3.so"
            includes+=" -Iext/sdl3/include"
            if [[ ${sdl_mixer} == "1" ]]; then
                includes+=" -Iext/sdl3_mixer/include"
                # libs+=" -lSDL3_mixer"
                libs+=" ${library}/libSDL3_mixer.so"
            fi
        fi
    #elif [[ ${sdl_source} == "1" ]]; then
        # libs+=" -Lext/sdl/build -Lext/sdl_image/build -Lext/sdl_mixer/build"
    #    libs+=" -static bin/libSDL2_x64.a bin/libSDL2_image_x64.a bin/libSDL2_mixer_x64.a"
    #    includes+=" -Iext/sdl/include -Iext/sdl_image/include -Iext/sdl_mixer/include"
    #    dflags+=" -Dsdlsource"
    else
        if [[ ${is_static} == "0" ]]; then
            echo "+ Using Systems SDL2"
            libs+=" -lSDL2"
            if [[ ${sdl_images} == "1" ]]; then
                libs+="  -lSDL2_image"
            fi
            if [[ ${sdl_mixer} == "1" ]]; then
                libs+=" -lSDL2_mixer"
            fi
        else
            libs+=" ${library}/libSDL2.so"
            includes+=" -Iext/sdl2/include"
            if [[ ${sdl_mixer} == "1" ]]; then
                includes+=" -Iext/sdl2_mixer/include"
                # libs+=" -lSDL3_mixer"
                libs+=" ${library}/libSDL2_mixer.so"
            fi
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
if [[ ${package} == "1" ]]; then
    mkdir -p ${package_path}
    date_str=$(date +%Y_%m_%d)
    zip_name="${package_path}/${game_name}_${OS}_${ARC}_${GLB}_${GFX}_${date_str}.zip"
    echo ""
    echo "> Packaging"
    echo "  - Zip [${zip_name}]"
    echo "  - Lib [${library}]"
    # removes old
    rm -f ${zip_name}
    # create new zip and add files
    zip -j "${zip_name}" "${bin_path}"
    zip -q -r "${zip_name}" res
    if [[ ${is_static} == "1" ]]; then
        if [[ ${is_sdl3} == "1" ]]; then
            zip -j "${zip_name}" ${library}/libSDL3.so.0
            if [[ ${sdl_mixer} == "1" ]]; then
                zip -j "${zip_name}" ${library}/libSDL3_mixer.so.0
            fi
        fi
    fi
    echo "+ Completed Zipping"
fi

#elif [[ ${ONARC} == "aarch64" && ${ARC} == "x64" ]]; then
#    echo "Cross Compiler Set"
#    echo "sudo apt install gcc-x86-64-linux-gnu"
#    compiler="x86_64-linux-gnu-gcc"
#    cflags+=" -march=x86-64"
#    sdl_source="1"
#   bsh/libs-download.sh
#   bsh/libs-compile-linux-x64.sh