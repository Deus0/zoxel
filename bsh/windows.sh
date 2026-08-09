#!/usr/bin/env bash
set -euo pipefail

# NOTE: Requires: sudo apt install gcc-mingw-w64-i686
# NOTE: This uses GLEW source code
# NOTE: This compiles SDL libraries with cmake and then links them

# Debug Options
is_safety_checks="1"
is_sdl_image="0"
is_sdl_mixer="1"

# Our  Libs
bsh/libs-download.sh --sdl-mixer
bsh/libs-compile2.sh windows x86_64  --sdl-mixer

# bash inputs
game_name=$1
bin_path="bin/${game_name}.exe"
OS="windows"
ARC="x64"
GLB="opengl"
GFX="sdl"
debug="False"
package="False"
sources="src/main.c inc/flecs/flecs.c"
includes="-Iinc/flecs"
cflags="-std=gnu99 -fPIC"
dflags="-Dzox_game=${game_name} -Dflecssource -Dzox_${OS}"
libs="-lws2_32 -ldbghelp -lpthread"
IS_GLEW="True"
compiler="i686-w64-mingw32-gcc"

[[ " $* " == *" --package "* ]] && package="True"
[[ " $* " == *" --debug "* ]] && debug="True"
[[ " $* " == *" --development "* ]] && debug="True"

if [[ ${debug} == "True" ]]; then
    cflags+=" -O0 -g3 -Wall -ggdb3 -Dzox_debug"
    bin_path="bin/${game_name}-dev.exe"
else
    cflags+=" -O3 -flto=auto -DNDEBUG"
fi

if [[ ${is_safety_checks} == "1" ]]; then
    echo "+ Added zox_safety_checks"
    dflags+=" -Dzox_safety_checks"
fi


if [[ ${IS_GLEW} == "True" ]]; then
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
    dflags+=" -Dzox_sdl -Dsdlsource"
    libs+=" -lSDL2"
    includes+=" -Iext/sdl/include"
    libs+=" -Lbin" # static libs for build
    if [[ ${is_sdl_image} == "1" ]]; then
        dflags+=" -Dzox_sdl_images"
        libs+=" -lSDL2_image"
        includes+=" -Iext/sdl_image/include"
    fi
    if [[ ${is_sdl_mixer} == "1" ]]; then
        dflags+=" -Dzox_sdl_mixer"
        libs+=" -lSDL2_mixer"
        includes+=" -Iext/sdl_mixer/include"
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

# ---- Packaging ----
if [[ ${package} == "True" ]]; then
    date_str=$(date +%Y_%m_%d)
    zip_name="bin/${game_name}_${OS}_${ARC}_${GLB}_${GFX}_${date_str}.zip"
    echo ""
    echo "Packaging [${zip_name}]"
    rm -f ${zip_name}
    zip -q -r "${zip_name}" res
    zip -j "${zip_name}" "${bin_path}"
    zip -j "${zip_name}" bin/SDL2.dll
    if [[ ${is_sdl_image} == "1" ]]; then
        zip -j "${zip_name}" bin/SDL2_image.dll
    fi
    if [[ ${is_sdl_mixer} == "1" ]]; then
        zip -j "${zip_name}" bin/SDL2_mixer.dll
    fi
    echo "Created package [${zip_name}]"
fi

# winedbg ?
#if [[ ${debug} == "True" ]]; then
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
