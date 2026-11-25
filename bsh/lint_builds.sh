#!/usr/bin/env bash
# Compile and link each supported desktop target with the available toolchains.
cd "$(dirname "$0")/.." || exit 2
source bsh/host.sh
source bsh/get-compiler.sh
failed=0
builds=0
check() {
    local name="$1"; shift
    builds=$((builds + 1))
    if bash bsh/build.sh zoxel --release --check "$@" --out "lint_${name}" > "tst/lint_build_${name}.log" 2>&1; then
        echo "+ [${name}] builds ($*)"
    else
        failed=$((failed + 1))
        echo "!! [${name}] doesn't build ($*), tst/lint_build_${name}.log:"
        grep -m 10 -E "error" "tst/lint_build_${name}.log"
    fi
}
mkdir -p tst
if [[ ${host_os} == "windows" ]]; then
    check windows_opengl --windows --opengl
    check windows_vulkan --windows --vulkan
else
    case "$(uname -m)" in
        x86_64) host_arch=x64 ;;
        aarch64) host_arch=arm ;;
        *) echo "!! unsupported architecture: $(uname -m)"; exit 2 ;;
    esac
    for arch in x64 arm; do
        compiler=$(get_compiler "${host_arch}" "${arch}" linux)
        if ! command -v "${compiler}" > /dev/null 2>&1; then
            echo "- [linux_${arch}] skipped, missing ${compiler}"
            continue
        fi
        check "linux_${arch}_headless" --linux "--${arch}" --headless
        check "linux_${arch}_opengl" --linux "--${arch}" --opengl
        if printf '#include <vulkan/vulkan.h>\n#include <shaderc/shaderc.h>\n' | "${compiler}" -E -x c - > /dev/null 2>&1; then
            check "linux_${arch}_vulkan" --linux "--${arch}" --vulkan
        else
            echo "- [linux_${arch}_vulkan] skipped, missing Vulkan or shaderc headers"
        fi
    done
    if command -v x86_64-w64-mingw32-gcc > /dev/null 2>&1 &&
       command -v x86_64-w64-mingw32-g++ > /dev/null 2>&1; then
        check windows_opengl --windows --x64 --opengl
        if [[ -f ext/vulkan_windows/mingw64/include/vulkan/vulkan.h ]]; then
            check windows_vulkan --windows --x64 --vulkan
        else
            echo "- [windows_vulkan] skipped, missing Windows Vulkan SDK files"
        fi
    else
        echo "- [windows] skipped, missing MinGW x64 toolchain"
    fi
fi
if [[ ${builds} == 0 ]]; then
    echo "[tst] [lint_builds] FAILED, no supported build toolchains found"
    exit 1
fi
echo "[tst] [lint_builds] $([[ ${failed} == 0 ]] && echo PASSED || echo FAILED) [${builds}] builds, [${failed}] failed"
[[ ${failed} == 0 ]]
