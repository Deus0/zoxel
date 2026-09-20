#!/usr/bin/env bash
set -euo pipefail

# Debug
#   bash bsh/web.sh zoxel
#
# Release
#   bash bsh/web.sh zoxel --release

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
date_str="$(date +%Y_%m_%d)"
game_name="${1:-zoxel}"
profile="debug"
run="0"
trace="0"
package="0"

[[ " $* " == *" --release "* ]] && profile="release"
[[ " $* " == *" --run "* ]] && run="1"
[[ " $* " == *" --trace "* ]] && trace="1"
[[ " $* " == *" --package "* ]] && package="1"

cd "${root}"

# ============================================================
# Paths
# ============================================================

web_path="${root}/bin/${game_name}_web"
src_path="${root}/src"
gam_path="${root}/gam/${game_name}"
flecs_path="${root}/inc/flecs"
main_file="${src_path}/main.c"
package_path="${root}/zip/${game_name}_web_${date_str}.zip"

# Emscripten SDK
emsdk_path="${root}/web/emsdk"

# ============================================================
# Emscripten
# ============================================================

if ! command -v git >/dev/null 2>&1; then
    echo "ERROR: git not found."
    exit 1
fi

if ! command -v python3 >/dev/null 2>&1; then
    echo "ERROR: python3 not found."
    exit 1
fi

if [[ ! -d "${emsdk_path}" ]]; then
    echo ""
    echo "> Installing Emscripten SDK"
    echo "  ${emsdk_path}"
    echo ""

    mkdir -p "$(dirname "${emsdk_path}")"

    git clone \
        https://github.com/emscripten-core/emsdk.git \
        "${emsdk_path}"
fi

cd "${emsdk_path}"

if [[ ! -x "./emsdk" ]]; then
    echo "ERROR: emsdk not found:"
    echo "  ${emsdk_path}/emsdk"
    exit 1
fi

if [[ ! -x "upstream/emscripten/emcc" ]]; then
    echo ""
    echo "> Installing latest Emscripten"
    ./emsdk install latest
    ./emsdk activate latest
fi

source ./emsdk_env.sh

cd "${root}"

if ! command -v emcc >/dev/null 2>&1; then
    echo "ERROR: emcc still not available."
    exit 1
fi

echo ""
echo "> Emscripten"
emcc --version | head -n 1

# ============================================================
# Check source
# ============================================================

if [[ ! -f "${main_file}" ]]; then
    echo "ERROR: Main source not found:"
    echo "  ${main_file}"
    exit 1
fi

if [[ ! -f "${flecs_path}/flecs.c" ]]; then
    echo "ERROR: Flecs source not found:"
    echo "  ${flecs_path}/flecs.c"
    exit 1
fi

if [[ ! -d "${gam_path}" ]]; then
    echo "ERROR: Game directory not found:"
    echo "  ${gam_path}"
    exit 1
fi

# ============================================================
# Flags
# ============================================================

cflags="-std=gnu99"
dflags="-Dzox_game=${game_name} -Dflecssource -Dzox_web -Dzox_sdl -Dzox_sdl3 -Dzox_opengl"
includes="-I${flecs_path} -I${src_path}"

if [[ "${profile}" == "debug" ]]; then
    echo "+ Debug"
    dflags+=" -Dzox_debug -Dzox_logs -Dzox_verbose"
    cflags+=" -Wall -g3 -O0"
else
    echo "+ Release"
    cflags+=" -O3 -DNDEBUG"
fi

trace_flags=()

if [[ "${trace}" == "1" ]]; then
    echo "+ Trace"
    trace_flags+=(
        "-g3"
        "-gsource-map=inline"
        "-sSTACK_OVERFLOW_CHECK=2"
        "-sASSERTIONS=2"
    )
fi

# ============================================================
# Build
# ============================================================

echo ""
echo "> Building WebGL"
echo "  Game: ${game_name}"
echo "  Output: ${web_path}"

rm -rf "${web_path}"
mkdir -p "${web_path}"

emcc \
    "${main_file}" \
    "${flecs_path}/flecs.c" \
    ${includes} \
    ${cflags} \
    ${dflags} \
    "${trace_flags[@]}" \
    -s USE_SDL=3 \
    -s USE_WEBGL2=1 \
    -s FULL_ES3=1 \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s STACK_SIZE=1048576 \
    --preload-file "${root}/res@/res" \
    -o "${web_path}/index.html"

# ============================================================
# Web canvas
# ============================================================

echo ""
echo "> Configuring WebGL canvas"

sed -i \
    's|</head>|<style>html, body { width: 100%; height: 100%; margin: 0; padding: 0; overflow: hidden; background: #000; } body > *:not(.emscripten_border) { display: none !important; } .emscripten, #status, #progress, #output, #controls { display: none !important; } .emscripten_border { position: fixed; inset: 0; width: 100% !important; height: 100% !important; border: 0 !important; margin: 0 !important; padding: 0 !important; } canvas.emscripten { width: 100% !important; height: 100% !important; display: block !important; border: 0 !important; }</style></head>|' \
    "${web_path}/index.html"

echo "- Canvas stretches to iframe"
echo "- Emscripten header hidden"

# ============================================================
# Resources
# ============================================================

echo ""
echo "> Copying resources"

mkdir -p "${web_path}/res"
cp -a "${root}/res/." "${web_path}/res/"

echo ""
echo "+ WebGL build complete"
echo "  ${web_path}"
echo ""

# ============================================================
# Package
# ============================================================

if [[ "${package}" == "1" ]]; then
    echo ""
    echo "> Packaging WebGL"

    if ! command -v zip >/dev/null 2>&1; then
        echo "ERROR: zip not found."
        exit 1
    fi

    mkdir -p "${root}/zip"
    rm -f "${package_path}"

    (
        cd "${web_path}"
        zip -r "${package_path}" .
    )

    echo ""
    echo "+ WebGL package created"
    echo "  ${package_path}"
fi

# ============================================================
# Run
# ============================================================

if [[ "${run}" == "1" ]]; then
    echo ""
    echo "> Running WebGL"

    if ! command -v emrun >/dev/null 2>&1; then
        echo "ERROR: emrun not found."
        exit 1
    fi

    cd "${web_path}"
    emrun index.html
fi