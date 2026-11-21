#!/usr/bin/env bash
# windows host (git bash / msys): finds or installs the toolchain, then puts it on the PATH
#   source bsh/deps_windows.sh     what build.sh does on a windows host (and web.sh, for python)
#   bash bsh/deps_windows.sh       just install
# winlibs (winget) is gcc with posix threads (build.sh links winpthread), and it brings cmake, ninja and make
#   for bsh/libs-compile.sh, so one package covers the whole build

winlibs_id="BrechtSanders.WinLibs.POSIX.UCRT"

source "$(dirname "${BASH_SOURCE[0]}")/host.sh"

# winget unzips winlibs into its packages folder: mingw64/bin holds gcc, cmake and ninja
find_winlibs_bin() {
    local packages
    packages="$(cygpath -u "${LOCALAPPDATA}")/Microsoft/WinGet/Packages"
    find "${packages}" -maxdepth 4 -type f -path "*${winlibs_id}*/mingw64/bin/gcc.exe" 2> /dev/null \
        | head -n 1 | xargs -r dirname
}

has_windows_toolchain() {
    command -v x86_64-w64-mingw32-gcc > /dev/null 2>&1 \
        && command -v x86_64-w64-mingw32-g++ > /dev/null 2>&1 \
        && command -v cmake > /dev/null 2>&1
}

install_windows_deps() {
    if ! is_windows_host; then
        echo "deps_windows.sh: not a windows host, skipping"
        return 0
    fi
    local winlibs_bin
    winlibs_bin="$(find_winlibs_bin)"
    if ! has_windows_toolchain && [[ -z "${winlibs_bin}" ]]; then
        if ! command -v winget > /dev/null 2>&1; then
            echo "ERROR: winget is needed to install [${winlibs_id}], or put a mingw-w64 gcc and cmake on the PATH"
            return 1
        fi
        echo "-> Installing [${winlibs_id}] with winget (gcc, cmake, ninja)"
        winget install --exact --id "${winlibs_id}" --silent \
            --accept-source-agreements --accept-package-agreements
        winlibs_bin="$(find_winlibs_bin)"
        if [[ -z "${winlibs_bin}" ]]; then
            echo "ERROR: installed [${winlibs_id}] but found no mingw64/bin/gcc.exe in winget's packages"
            return 1
        fi
    fi
    if [[ -n "${winlibs_bin}" && ":${PATH}:" != *":${winlibs_bin}:"* ]]; then
        export PATH="${winlibs_bin}:${PATH}"
    fi
    if ! has_windows_toolchain; then
        echo "ERROR: still missing x86_64-w64-mingw32-gcc, x86_64-w64-mingw32-g++ or cmake"
        return 1
    fi
    # cmake on windows defaults to visual studio, which isn't there
    export CMAKE_GENERATOR="${CMAKE_GENERATOR:-Ninja}"
    echo "+ Windows toolchain [$(command -v x86_64-w64-mingw32-gcc)]"
}

# python for emscripten (bsh/web.sh): emsdk and emcc are python scripts, they use EMSDK_PYTHON when it's set
#   windows' python3 on the PATH is often the store's stub (WindowsApps), which opens the store instead of running,
#   so a real one is found in the user's programs, or installed there with winget (user scope, no admin)
python_id="Python.Python.3.12"

find_windows_python() {
    local programs
    programs="$(cygpath -u "${LOCALAPPDATA}")/Programs/Python"
    find "${programs}" -maxdepth 2 -type f -name python.exe 2> /dev/null | sort -V | tail -n 1
}

install_windows_python() {
    if ! is_windows_host; then
        return 0
    fi
    local python
    python="$(find_windows_python)"
    if [[ -z "${python}" ]]; then
        if ! command -v winget > /dev/null 2>&1; then
            echo "ERROR: winget is needed to install [${python_id}], or set EMSDK_PYTHON to a python.exe"
            return 1
        fi
        echo "-> Installing [${python_id}] with winget (for emscripten)"
        winget install --exact --id "${python_id}" --scope user --silent \
            --accept-source-agreements --accept-package-agreements
        python="$(find_windows_python)"
        if [[ -z "${python}" ]]; then
            echo "ERROR: installed [${python_id}] but found no python.exe in ${LOCALAPPDATA}/Programs/Python"
            return 1
        fi
    fi
    export EMSDK_PYTHON="${python}"
    echo "+ Python [${python}]"
}

# run directly: install. sourced: build.sh calls install_windows_deps, web.sh install_windows_python
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    set -euo pipefail
    install_windows_deps
fi
