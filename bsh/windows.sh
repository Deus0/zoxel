#!/usr/bin/env bash
set -euo pipefail

# NOTE: Requires: sudo apt install gcc-mingw-w64-i686
# NOTE: This uses GLEW source code
# NOTE: This compiles SDL libraries with cmake and then links them

# Debug Options
is_safety_checks="1"
is_sdl_image="0"
sdl_mixer="1"
# bash inputs
game_name=$1
bin_path="bin/${game_name}.exe"
OS="windows"
ARC="x64"
GLB="opengl"
GFX="sdl"
sources="src/main.c inc/flecs/flecs.c"
includes="-Iinc/flecs"
cflags="-std=gnu99 -fPIC"
dflags="-Dzox_game=${game_name} -Dflecssource -Dzox_${OS}"
libs="-lws2_32 -ldbghelp -lpthread"
IS_GLEW="1"
compiler="x86_64-w64-mingw32-gcc" # "i686-w64-mingw32-gcc"
package_path="zip"
architecture="x64"   # base arch name
library="lib/${OS}_${architecture}"

debug="0"
package="0"
is_static="0"
is_sdl3="0"

[[ " $* " == *" --debug "* ]] && debug="1"
[[ " $* " == *" --package "* ]] && package="1"
[[ " $* " == *" --static "* ]] && is_static="1"
[[ " $* " == *" --sdl3 "* ]] && is_sdl3="1"

# Our  Libs
lib_args=""
[[ ${sdl_mixer} == "1" ]] && lib_args+=" --sdl-mixer"
[[ ${is_sdl3} == "1" ]] && lib_args+=" --sdl3"
bsh/libs-download.sh ${lib_args}
bsh/libs-compile.sh windows ${architecture} ${lib_args}

if [[ ${debug} == "1" ]]; then
    cflags+=" -O0 -g3 -Wall -ggdb3 -Dzox_debug"
    bin_path="bin/${game_name}-dev.exe"
else
    cflags+=" -O3 -flto=auto -DNDEBUG"
fi

if [[ ${is_safety_checks} == "1" ]]; then
    echo "+ Added zox_safety_checks"
    dflags+=" -Dzox_safety_checks"
fi


if [[ ${IS_GLEW} == "1" ]]; then
    echo "+ Added Glew"
    sources+=" ext/glew/src/glew.c"
    dflags+=" -DGLEW_STATIC"
    includes+=" -Iext/glew/include"
fi

if [[ ${GLB} == "opengl" ]]; then
    echo "+ Added [opengl]"
    dflags+=" -Dzox_opengl"
    libs+=" -lopengl32"
fi

if [[ ${GFX} == "sdl" ]]; then
    echo "+ Added [sdl]"
    dflags+=" -Dzox_sdl"
    dflags+=" -Dsdlsource"
    # libs+=" -L${library}" # static libs for build
    if [[ ${is_sdl3} == "1" ]]; then
        echo "+ Added [sdl3]"
        dflags+=" -Dzox_sdl3"
        # libs+=" -lSDL3"
        libs+=" ${library}/SDL3.dll"
        includes+=" -Iext/sdl3/include"
        if [[ ${sdl_mixer} == "1" ]]; then
            includes+=" -Iext/sdl3_mixer/include"
            libs+=" ${library}/SDL3_mixer.dll"
        fi
    else
        # libs+=" -lSDL2"
        libs+=" ${library}/SDL2.dll"
        includes+=" -Iext/sdl2/include"
        if [[ ${sdl_mixer} == "1" ]]; then
            dflags+=" -Dzox_sdl_mixer"
            libs+=" ${library}/SDL2_mixer.dll"
            # libs+=" -lSDL2_mixer"
            includes+=" -Iext/sdl_mixer/include"
        fi
        if [[ ${is_sdl_image} == "1" ]]; then
            dflags+=" -Dzox_sdl_images"
            libs+=" -lSDL2_image"
            includes+=" -Iext/sdl_image/include"
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
${compiler} ${cflags} ${sources} -o "${bin_path}" ${includes} ${libs} ${dflags}
echo "+ Build Completed"

# ---- Packaging ----
if [[ ${package} == "1" ]]; then
    mkdir -p $package_path
    date_str=$(date +%Y_%m_%d)
    zip_name="${package_path}/${game_name}_${OS}_${ARC}_${GLB}_${GFX}_${date_str}.zip"

    echo ""
    echo "> Packaging"
    echo "  - Zip [${zip_name}]"
    echo "  - Lib [${library}]"

    rm -f ${zip_name}
    zip -q -r "${zip_name}" res
    zip -j "${zip_name}" "${bin_path}"
    if [[ ${is_sdl3} == "1" ]]; then
        zip -j "${zip_name}" ${library}/SDL3.dll
    else
        zip -j "${zip_name}" ${library}/SDL2.dll
        if [[ ${is_sdl_image} == "1" ]]; then
            zip -j "${zip_name}" ${library}/SDL2_image.dll
        fi
        if [[ ${sdl_mixer} == "1" ]]; then
            zip -j "${zip_name}" ${library}/SDL2_mixer.dll
        fi
    fi

    echo "Created package [${zip_name}]"
fi

# winedbg ?
#if [[ ${debug} == "1" ]]; then
    # WINEDEBUG=+backtrace wine bin/${bin}
    # WINEDEBUG=-dbghelp wine bin/${bin}
    # echo "Enter 'Continue' after it loads."
#    winedbg --gdb ${bin_path}
#else
#    wine ${bin_path}
#fi
#-Lext/sdl/build \
#-Lext/sdl_image/build \
#-Lext/sdl_mixer/build \
