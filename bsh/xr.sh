#!/usr/bin/env bash
set -euo pipefail

# ============================================================
# OpenXR Android setup
#
# Usage:
#
#   bash bsh/android.sh zoxel opengl sdl --debug --xr
#
# Downloads OpenXR headers and the Android OpenXR loader.
#
# OpenXR files are cached in:
#
#   ext/xr/
#
# ============================================================

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "${root}"

xr_version="1.1.61"

xr_path="${root}/ext/xr"
xr_include_path="${xr_path}/include"

xr_archive="${root}/ext/openxr-sdk-${xr_version}.tar.gz"
xr_url="https://github.com/KhronosGroup/OpenXR-SDK/archive/refs/tags/release-${xr_version}.tar.gz"

xr_android_path="${xr_path}/android"
xr_aar="${root}/ext/openxr-loader-${xr_version}.aar"
xr_aar_url="https://repo1.maven.org/maven2/org/khronos/openxr/openxr_loader_for_android/${xr_version}/openxr_loader_for_android-${xr_version}.aar"

xr_loader_path="${xr_android_path}/arm64-v8a"
xr_loader="${xr_loader_path}/libopenxr_loader.so"

tmp_dir=""
tmp_loader_dir=""

trap 'rm -rf "${tmp_dir:-}" "${tmp_loader_dir:-}" "${xr_archive:-}" "${xr_aar:-}"' EXIT

# ============================================================
# Download OpenXR headers
# ============================================================

echo ""
echo "> OpenXR ${xr_version}"

if [[ ! -f "${xr_include_path}/openxr/openxr.h" ]]; then

    echo "- OpenXR headers not found"
    echo "- Downloading:"
    echo "  ${xr_url}"

    mkdir -p "${root}/ext"

    curl \
        --fail \
        --location \
        --retry 3 \
        --output "${xr_archive}" \
        "${xr_url}"

    echo "- Extracting OpenXR"

    tmp_dir="$(mktemp -d)"

    tar \
        -xzf "${xr_archive}" \
        -C "${tmp_dir}"

    extracted_dir="$(find "${tmp_dir}" -mindepth 1 -maxdepth 1 -type d | head -n 1)"

    if [[ -z "${extracted_dir}" ]]; then
        echo ""
        echo "ERROR: OpenXR archive extraction failed."
        exit 1
    fi

    rm -rf "${xr_path}"
    mkdir -p "${xr_path}"

    cp -a \
        "${extracted_dir}/include/." \
        "${xr_include_path}/"

    echo "- OpenXR headers installed:"
    echo "  ${xr_include_path}"

else

    echo "- OpenXR headers already installed:"
    echo "  ${xr_include_path}"

fi

if [[ ! -f "${xr_include_path}/openxr/openxr.h" ]]; then
    echo ""
    echo "ERROR: OpenXR header missing:"
    echo "  ${xr_include_path}/openxr/openxr.h"
    exit 1
fi

echo "+ OpenXR headers OK"

# ============================================================
# Download Android OpenXR loader
# ============================================================

echo ""
echo "> OpenXR Android loader"

if [[ ! -f "${xr_loader}" ]]; then

    echo "- OpenXR Android loader not found"
    echo "- Downloading:"
    echo "  ${xr_aar_url}"

    mkdir -p "${root}/ext"

    curl \
        --fail \
        --location \
        --retry 3 \
        --output "${xr_aar}" \
        "${xr_aar_url}"

    echo "- Extracting Android loader"

    tmp_loader_dir="$(mktemp -d)"

    unzip -q \
        "${xr_aar}" \
        "jni/arm64-v8a/libopenxr_loader.so" \
        -d "${tmp_loader_dir}"

    if [[ ! -f "${tmp_loader_dir}/jni/arm64-v8a/libopenxr_loader.so" ]]; then
        echo ""
        echo "ERROR: OpenXR Android loader missing from AAR:"
        echo "  ${xr_aar}"
        exit 1
    fi

    rm -rf "${xr_android_path}"
    mkdir -p "${xr_loader_path}"

    cp \
        "${tmp_loader_dir}/jni/arm64-v8a/libopenxr_loader.so" \
        "${xr_loader}"

    echo "- OpenXR loader installed:"
    echo "  ${xr_loader}"

else

    echo "- OpenXR Android loader already installed:"
    echo "  ${xr_loader}"

fi

if [[ ! -f "${xr_loader}" ]]; then
    echo ""
    echo "ERROR: OpenXR loader missing:"
    echo "  ${xr_loader}"
    exit 1
fi

echo "+ OpenXR Android loader OK"

# ============================================================
# Android project setup
# ============================================================

staging_path="${root}/and/zoxel"

if [[ ! -d "${staging_path}" ]]; then
    echo ""
    echo "ERROR: Android staging project does not exist:"
    echo "  ${staging_path}"
    exit 1
fi



# ============================================================
# OpenXR Android manifest
# ============================================================

manifest_path="${staging_path}/app/src/main/AndroidManifest.xml"

if [[ ! -f "${manifest_path}" ]]; then
    echo ""
    echo "ERROR: AndroidManifest.xml not found:"
    echo "  ${manifest_path}"
    exit 1
fi

echo ""
echo "> Configuring OpenXR manifest"

if ! grep -q 'org.khronos.openxr.intent.category.IMMERSIVE_HMD' "${manifest_path}"; then
    sed -i \
        '/<category android:name="android.intent.category.LAUNCHER" \/>/a\
            <category android:name="org.khronos.openxr.intent.category.IMMERSIVE_HMD" />' \
        "${manifest_path}"
fi

if ! grep -q 'com.oculus.intent.category.VR' "${manifest_path}"; then
    sed -i \
        '/<category android:name="org.khronos.openxr.intent.category.IMMERSIVE_HMD" \/>/a\
            <category android:name="com.oculus.intent.category.VR" />' \
        "${manifest_path}"
fi

echo "- OpenXR manifest configured"

# would be good if we can
# ============================================================
# OpenXR hand tracking
# ============================================================

echo ""
echo "> Configuring OpenXR hand tracking"


echo "- OpenXR hand tracking configured"

# ============================================================
# OpenXR JNI Config
# ============================================================

jni_path="${staging_path}/app/jni"
jni_xr_path="${jni_path}/xr"

echo ""
echo "> Preparing OpenXR Android project"

rm -rf "${jni_xr_path}"

mkdir -p "${jni_xr_path}"

cp -a \
    "${xr_include_path}" \
    "${jni_xr_path}/"

mkdir -p "${jni_xr_path}/android/arm64-v8a"

cp \
    "${xr_loader}" \
    "${jni_xr_path}/android/arm64-v8a/libopenxr_loader.so"

echo "- OpenXR headers staged:"
echo "  ${jni_xr_path}/include"

echo "- OpenXR loader staged:"
echo "  ${jni_xr_path}/android/arm64-v8a/libopenxr_loader.so"

# ============================================================
# Done
# ============================================================

echo ""
echo "============================================================"
echo "+ OpenXR Android preparation complete"
echo "============================================================"
echo ""
echo "OpenXR version:"
echo "  ${xr_version}"
echo ""
echo "Headers:"
echo "  ${xr_include_path}"
echo ""
echo "Android loader:"
echo "  ${xr_loader}"
echo ""
echo "Native include:"
echo "  ${jni_xr_path}/include"
echo ""