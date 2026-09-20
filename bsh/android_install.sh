#!/usr/bin/env bash
set -euo pipefail

game_name="${1:-zoxel}"
root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
apk_dir="${root}/zip"

if ! command -v adb >/dev/null 2>&1; then
    echo "ERROR: adb not found."
    exit 1
fi

apk_path="$(
    find "${apk_dir}" \
        -maxdepth 1 \
        -type f \
        -name "${game_name}_android_arm*.apk" \
        -printf '%T@ %p\n' |
    sort -nr |
    head -n 1 |
    cut -d' ' -f2-
)"

if [[ -z "${apk_path}" || ! -f "${apk_path}" ]]; then
    echo "ERROR: No APK found for ${game_name}."
    echo "  ${apk_dir}/${game_name}_android_arm*.apk"
    exit 1
fi

if [[ -z "$(adb devices | awk 'NR > 1 && $2 == "device" {print $1}')" ]]; then
    echo "ERROR: No Android device connected."
    exit 1
fi

echo "> Installing"
echo "  ${apk_path}"

adb install -r "${apk_path}"

echo "+ APK installed"