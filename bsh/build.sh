#!/usr/bin/env bash
set -euo pipefail

# NOTE: Converts the shell flags into flags for our game code
source bsh/get-compiler.sh

# settings
game_name="zoxel"
os="linux"
on_arc=$(uname -m)
if [[ ${on_arc} == "aarch64" ]]; then
    on_arc="arm"
elif [[ ${on_arc} == "x86_64" ]]; then
    on_arc="x64"
fi
arc="${on_arc}"         # x64 or arm
# compiler="gcc"
cflags="-std=gnu99 -fPIC"
dflags="-Dflecssource"
debug="0"
is_safety_checks="1"    # lets stay safe for now
is_profiler="0"         # https://www.flecs.dev/explorer/?host=localhost
is_fast_dev="0"         # -O3
is_run="0"
logs="0"
verbose="0"
package="0"
server="0"
is_mesa="0"
docker="0"
is_time_systems="0"

# Libs
libs="-lpthread"            # shared
is_static="1"
graphics_lib="opengl"       # headless, opengl or vulkan
window_lib="sdl"            # sdl, glut
is_sdl3="1"                 # sdl2, sdl3
sdl_mixer="1"
is_glew="0"
is_desktop_gl="1"           # Use GL libs instead of EGL on desktop
native="0"                  # Fastest

# Paths
sources="src/main.c inc/flecs/flecs.c"
includes="-Iinc/flecs"
output_folder="bin"
package_path="zip"
output_extension="bin"

# Parse our Arguments #

# if not a -- we set our game name
if [[ $# -gt 0 && ${1} != --* ]]; then
    game_name="$1"
fi

# Platform
[[ " $* " == *" --linux "* ]] && os="linux"
[[ " $* " == *" --windows "* ]] && os="windows"

# Architecture
[[ " $* " == *" --x64 "* ]] && arc="x64"
[[ " $* " == *" --arm "* ]] && arc="arm"
[[ " $* " == *" --native "* ]] && native="1"

# Libraries
[[ " $* " == *" --nomixer "* ]] && sdl_mixer="0"
[[ " $* " == *" --system "* ]] && is_static="0"
[[ " $* " == *" --package "* ]] && package="1"
[[ " $* " == *" --static "* ]] && is_static="1"

# window_lib
[[ " $* " == *" --headless "* ]] && window_lib="headless"
[[ " $* " == *" --sdl "* ]] && window_lib="sdl"
[[ " $* " == *" --glut "* ]] && window_lib="glut"
[[ " $* " == *" --sdl2 "* ]] && is_sdl3="0"
[[ " $* " == *" --sdl3 "* ]] && is_sdl3="1"

# Graphics Library
[[ " $* " == *" --headless "* ]] && graphics_lib="headless"
[[ " $* " == *" --opengl "* ]] && graphics_lib="opengl"
[[ " $* " == *" --vulkan "* ]] && graphics_lib="vulkan"

# Misc
[[ " $* " == *" --debug "* ]] && debug="1"
[[ " $* " == *" --release "* ]] && debug="0"
[[ " $* " == *" --run "* ]] && is_run="1"
[[ " $* " == *" --nologs "* ]] && logs="0"
[[ " $* " == *" --logs "* ]] && logs="1"
[[ " $* " == *" --untimed "* ]] && is_time_systems="0"
[[ " $* " == *" --timings "* ]] && is_time_systems="1"
[[ " $* " == *" --profile "* ]] && is_profiler="1"
[[ " $* " == *" --verbose "* ]] && verbose="1"
[[ " $* " == *" --server "* ]] && server="1"
[[ " $* " == *" --mesa "* ]] && is_mesa="1"
[[ " $* " == *" --docker "* ]] && docker="1"


# Change based on Parsed Args #

bin_filename="${game_name}"
dflags+=" -Dzox_game=${game_name}"

if [[ "${os}" == "linux" ]]; then
    output_extension="bin"
elif [[ "${os}" == "windows" ]]; then
    output_extension="exe"
fi

[[ "${graphics_lib}" == "headless" ]] && bin_filename="${bin_filename}-headless"
[[ "${debug}" == "1" ]] && bin_filename="${bin_filename}-dev"
bin_path="${output_folder}/${bin_filename}.${output_extension}"

# Set our compiler variables #
compiler="$(get_compiler "$on_arc" "$arc" "$os")"

if [[ "${os}" == "linux" ]]; then
    dflags+=" -Dzox_linux"
    libs+=" -lm -ldl"
elif [[ "${os}" == "windows" ]]; then
    is_glew="1"
    # compiler="x86_64-w64-mingw32-gcc"
    dflags+=" -Dzox_windows"
    libs+=" -lws2_32 -ldbghelp"
    if ! command -v x86_64-w64-mingw32-gcc >/dev/null 2>&1; then
        echo "ERROR: MinGW x64 toolchain is required for Windows builds."
        echo "Missing: x86_64-w64-mingw32-gcc"
        exit 1
    fi
    if ! command -v x86_64-w64-mingw32-g++ >/dev/null 2>&1; then
        echo "ERROR: MinGW x64 toolchain is required for Windows builds."
        echo "Missing: x86_64-w64-mingw32-g++"
        echo "Apt: sudo apt install g++-mingw-w64-x86-64"
        exit 1
    fi
    if [[ "${arc}" != "x64" ]]; then
        echo "ERROR: Windows builds currently support x64 only."
        exit 1
    fi
fi


if [[ ${arc} == "arm" ]]; then
    echo "+ [Arm] Enabled [gles]"
    is_desktop_gl="0"
fi

if [[ ${debug} == "1" ]]; then
    echo "+ Added [zox_debug]"
    dflags+=" -Dzox_debug"
    # cflags="-O2 -g -Dzox_debug"
    # cflags="-fPIC -g3 -Dzox_debug" #  -O0
    # For Regular Runs
    if [[ ${is_fast_dev} == "1" ]]; then
        echo "+ Added [fast_debug]"
        cflags+=" -O3 -g"
    else
        cflags+=" -O0 -g3"
        cflags+=" -Wall -ggdb3"
    fi
    # Memory Leaks Full Debug
    # cflags+=" -fno-omit-frame-pointer""
    # cflags+=" -fsanitize=address"
else
    # Release Builds
    cflags+=" -O3 -flto=auto -DNDEBUG"
fi

if [[ ${native} == "1" ]]; then
    cflags+=" -march=native"
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

if [[ ${graphics_lib} == "headless" ]]; then
    echo "+ Added [zox_headless]"
    dflags+=" -Dzox_headless"
fi

if [[ ${server} == "1" ]]; then
    echo "+ Added [zox_server]"
    dflags+=" -Dzox_server"
fi

if [[ ${graphics_lib} == "opengl" ]]; then
    echo "+ Added [zox_opengl]"
    dflags+=" -Dzox_opengl"
fi

if [[ ${window_lib} == "sdl" ]]; then
    echo "+ Added [zox_sdl]"
    dflags+=" -Dzox_sdl"
fi

# Construct our Librarys
library="lib/${os}_${arc}"
sdl_include=""
sdl_library=""
sdl_runtime=""
sdl_mixer_include=""
sdl_mixer_library=""
sdl_mixer_runtime=""

[[ "${docker}" == "1" ]] && library="${library}_docker"

if [[ "${is_sdl3}" == "1" ]]; then
    sdl_include="-Iext/sdl3/include"
    sdl_mixer_include="-Iext/sdl3_mixer/include"
    if [[ "${os}" == "windows" ]]; then
        sdl_library="${library}/SDL3.dll"
        sdl_runtime="${sdl_library}"
        sdl_mixer_library="${library}/SDL3_mixer.dll"
        sdl_mixer_runtime="${sdl_mixer_library}"
    else
        sdl_library="${library}/libSDL3.so"
        sdl_mixer_library="${library}/libSDL3_mixer.so"
        sdl_runtime="${library}/libSDL3.so.0"
        sdl_mixer_runtime="${library}/libSDL3_mixer.so.0"
    fi
else
    sdl_include="-Iext/sdl2/include"
    sdl_mixer_include="-Iext/sdl2_mixer/include"
    if [[ "${os}" == "windows" ]]; then
        sdl_library="${library}/SDL2.dll"
        sdl_runtime="${sdl_library}"
        sdl_mixer_library="${library}/SDL2_mixer.dll"
        sdl_mixer_runtime="${sdl_mixer_library}"
    else
        sdl_library="${library}/libSDL2.so"
        sdl_runtime="${sdl_library}.0"
        sdl_mixer_library="${library}/libSDL2_mixer.so"
        sdl_mixer_runtime="${sdl_mixer_library}.0"
    fi
fi

# Bind our Libraries

if [[ ${is_glew} == "1" ]]; then
    echo "+ Added [glew]"
    sources+=" ext/glew/src/glew.c"
    dflags+=" -DGLEW_STATIC"
    includes+=" -Iext/glew/include"
fi

if [[ ${graphics_lib} == "opengl" ]]; then
    if [[ ${os} == "windows" ]]; then
        libs+=" -lopengl32"
    elif [[ ${is_desktop_gl} == "1" ]]; then
        # Special case when compiling from arm to x64
        if [[ ${arc} == "x64" && ${on_arc} == "arm" ]]; then
            libs+=" -lEGL -lGLESv2"
        else
            libs+=" -lGL"
        fi
    else
        libs+=" -lEGL -lGLESv2"
    fi
fi

if [[ ${window_lib} == "sdl" ]]; then
    # libs+=" -L${library}" # static libs for build
    if [[ ${sdl_mixer} == "1" ]]; then
        dflags+=" -Dzox_sdl_mixer"
    fi
    if [[ "${is_sdl3}" == "1" ]]; then
        echo "+ Added [zox_sdl3]"
        dflags+=" -Dzox_sdl3"
    fi
    if [[ ${is_static} == "1" ]]; then
        libs+=" ${sdl_library}"
        includes+=" ${sdl_include}"
        if [[ "${sdl_mixer}" == "1" ]]; then
            libs+=" ${sdl_mixer_library}"
            includes+=" ${sdl_mixer_include}"
        fi
    else
        echo "+ Using Systems SDL"
        if [[ ${is_sdl3} == "1" ]]; then
            libs+=" -lSDL3"
            if [[ ${sdl_mixer} == "1" ]]; then
                libs+=" -lSDL3_mixer"
            fi
        else
            libs+=" -lSDL2"
            if [[ ${sdl_mixer} == "1" ]]; then
                libs+=" -lSDL2_mixer"
            fi
        fi
    fi
fi

echo "Chosen Arc [${arc}] - Running on [${on_arc}]"

# Ready our Libs
if [[ ${is_static} == "1" ]]; then
    # Make use local lib files during runtime
    if [[ "${os}" == "linux" ]]; then
        libs+=" -Wl,-rpath,\$ORIGIN"
    fi
fi

if [[ "${window_lib}" == "sdl" && ${is_static} == "1" ]]; then
    echo ""
    echo "--------------------------------------------"
    lib_args=""
    [[ ${is_sdl3} == "0" ]] && lib_args+=" --sdl2"
    [[ ${is_sdl3} == "1" ]] && lib_args+=" --sdl3"
    [[ ${sdl_mixer} == "1" ]] && lib_args+=" --sdl-mixer"
    [[ ${docker} == "1" ]] && lib_args+=" --docker"
    [[ ${is_mesa} == "1" ]] && lib_args+=" --mesa"
    bsh/libs-download.sh ${lib_args}
    echo "--------------------------------------------"
    bsh/libs-compile.sh ${os} --${arc} ${lib_args}
    echo "--------------------------------------------"
    echo ""
fi

clear 2>/dev/null || true
echo ""
echo "============================================================"
echo "                         Z O X E L"
echo "                    Universal Build System"
echo "------------------------------------------------------------"
echo "  Game     : ${game_name}"
echo "  Platform : ${os}"
echo "  Arch     : ${arc}"
echo "  Graphics : ${graphics_lib}"
echo "  Window   : ${window_lib}"
echo "  SDL      : $(if [[ ${is_sdl3} == "1" ]]; then echo "SDL3"; else echo "SDL2"; fi)"
echo "  Build    : $(if [[ ${debug} == "1" ]]; then echo "Debug"; else echo "Release"; fi)"
echo "  Compiler : ${compiler}"

[[ ${is_run} == "1" ]] && echo "  Run      : enabled"
[[ ${logs} == "1" ]] && echo "  Logs     : enabled"
[[ ${docker} == "1" ]] && echo "  Docker   : enabled"
[[ ${is_profiler} == "1" ]] && echo "  Profiler : enabled"
[[ ${is_time_systems} == "1" ]] && echo "  Timed    : enabled"

echo "============================================================"
echo ""

# echo ""
# echo "Building [${bin_path}]"
# echo "  - Compiler [${compiler}]"
# echo "  - CFlags [${cflags}]"
# echo "  - DFlags [${dflags}]"
#echo "  - Libs [${libs}]"
#echo "  - Includes [${includes}]"

mkdir -p ${output_folder}
${compiler} ${cflags} ${sources} -o "${bin_path}" ${includes} ${dflags} ${libs}
echo "+ Completed Build [${bin_path}]"

# ---- Packaging ----
if [[ ${package} == "1" ]]; then
    mkdir -p ${package_path}
    date_str=$(date +%Y_%m_%d)
    zip_name="${package_path}/${game_name}_${os}_${arc}_${graphics_lib}_${window_lib}"
    [[ ${window_lib} == "headless" ]] && zip_name="${zip_name}_headless"
    zip_name="${zip_name}_${date_str}.zip"

    echo ""
    echo "> Packaging"
    echo "  - Zip [${zip_name}]"
    echo "  - Lib [${library}]"

    rm -f ${zip_name}

    zip -j "${zip_name}" "${bin_path}"
    zip -q -r "${zip_name}" res

    if [[ -d "gam/${game_name}/res" ]]; then
        echo "> Using Game Resources [gam/${game_name}/res]"
        (
            cd "gam/${game_name}"
            zip -q -r "${OLDPWD}/${zip_name}" res
        )
    fi

    if [[ ${window_lib} == "sdl" ]]; then
        if [[ "${is_static}" == "1" ]]; then
            zip -j "${zip_name}" "${sdl_runtime}"

            if [[ "${sdl_mixer}" == "1" ]]; then
                zip -j "${zip_name}" "${sdl_mixer_runtime}"
            fi
        fi
    fi
    echo "+ Completed Zipping"
fi

# Run
if [[ "${is_run}" == "1" ]]; then
    echo "Running Game [${bin_path}]"
    ./"${bin_path}"
fi
