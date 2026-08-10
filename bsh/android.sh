#!/usr/bin/env bash
set -euo pipefail

# Test
#    bash bsh/android.sh zoxel opengl sdl --debug --install
# Debug
#    bash bsh/android.sh zoxel opengl sdl --debug --install --log --verbose
# ============================================================
# Zoxel Android Build
#
# Builds Zoxel into an Android APK using:
#
# SDL3 Android project
# Android SDK
# Android NDK
# Local Gradle
# JDK 17
#
# Environment is prepared by:
#
#   bash bsh/android_prepare.sh
#
# Output:
#
#   bin/zoxel.apk
#
# Staging:
#
#   and/zoxel/
#
# The staging directory is recreated for each build.
# ============================================================

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "${root}"

game_name="${1:-zoxel}"

debug="False"
install="False"
log="False"
verbose="False"

[[ " $* " == *" --debug "* ]] && debug="True"
[[ " $* " == *" --install "* ]] && install="True"
[[ " $* " == *" --log "* ]] && log="True"
[[ " $* " == *" --verbose "* ]] && verbose="True"

# ============================================================
# Android configuration
# ============================================================

android_api="35"
android_abi="arm64-v8a"

and_path="${root}/and"

sdk_path="${and_path}/sdk"
ndk_path="${and_path}/ndk"

gradle_path="${and_path}/gradle"
gradle_version="8.10.2"

staging_path="${and_path}/${game_name}"

apk_path="${root}/bin/${game_name}.apk"

# ============================================================
# JDK 17
# ============================================================

java17_path="/usr/lib/jvm/java-17-openjdk"

if [[ ! -x "${java17_path}/bin/java" ]]; then
    echo ""
    echo "ERROR: JDK 17 not found."
    echo ""
    echo "Run:"
    echo "  bash bsh/android_prepare.sh"
    echo ""
    exit 1
fi

if [[ ! -x "${java17_path}/bin/javac" ]]; then
    echo ""
    echo "ERROR: JDK 17 javac not found."
    echo ""
    echo "Run:"
    echo "  bash bsh/android_prepare.sh"
    echo ""
    exit 1
fi

export JAVA_HOME="${java17_path}"
export PATH="${JAVA_HOME}/bin:${PATH}"

echo ""
echo "> Java"
echo "  JAVA_HOME=${JAVA_HOME}"

java -version 2>&1 | sed 's/^/  /'

java_major="$(java -version 2>&1 | sed -n 's/.*version "\([0-9]*\).*/\1/p' | head -n 1)"

if [[ "${java_major}" != "17" ]]; then
    echo ""
    echo "ERROR: Android build requires JDK 17."
    echo "Found Java ${java_major}."
    exit 1
fi

echo "- JDK 17 selected"

# ============================================================
# Android environment
# ============================================================

echo ""
echo "> Checking Android environment"

if [[ ! -d "${sdk_path}" ]]; then
    echo ""
    echo "ERROR: Android SDK not found:"
    echo "  ${sdk_path}"
    echo ""
    echo "Run:"
    echo "  bash bsh/android_prepare.sh"
    exit 1
fi

if [[ ! -d "${ndk_path}" ]]; then
    echo ""
    echo "ERROR: Android NDK not found:"
    echo "  ${ndk_path}"
    echo ""
    echo "Run:"
    echo "  bash bsh/android_prepare.sh"
    exit 1
fi

gradle_bin="${gradle_path}/gradle-${gradle_version}/bin/gradle"

if [[ ! -x "${gradle_bin}" ]]; then
    echo ""
    echo "ERROR: Gradle not found:"
    echo "  ${gradle_bin}"
    echo ""
    echo "Run:"
    echo "  bash bsh/android_prepare.sh"
    exit 1
fi

sdkmanager_path="${sdk_path}/cmdline-tools/latest/bin/sdkmanager"

if [[ ! -x "${sdkmanager_path}" ]]; then
    echo ""
    echo "ERROR: Android SDK command-line tools not found."
    echo ""
    echo "Run:"
    echo "  bash bsh/android_prepare.sh"
    exit 1
fi

export ANDROID_HOME="${sdk_path}"
export ANDROID_SDK_ROOT="${sdk_path}"
export ANDROID_NDK_HOME="${ndk_path}"

export PATH="${sdk_path}/cmdline-tools/latest/bin:${sdk_path}/platform-tools:${PATH}"

echo "- SDK    ${sdk_path}"
echo "- NDK    ${ndk_path}"
echo "- Gradle ${gradle_bin}"

# ============================================================
# Source tree
# ============================================================

echo ""
echo "> Checking Zoxel source"

src_path="${root}/src"
gam_path="${root}/gam/${game_name}"
flecs_path="${root}/inc/flecs"
sdl_path="${root}/ext/sdl3"

sources=(
    "${src_path}/main.c"
    "${flecs_path}/flecs.c"
)

for source in "${sources[@]}"; do
    if [[ ! -f "${source}" ]]; then
        echo ""
        echo "ERROR: Missing source:"
        echo "  ${source}"
        exit 1
    fi
done

if [[ ! -d "${src_path}" ]]; then
    echo ""
    echo "ERROR: Source directory not found:"
    echo "  ${src_path}"
    exit 1
fi

if [[ ! -d "${gam_path}" ]]; then
    echo ""
    echo "ERROR: Game directory not found:"
    echo "  ${gam_path}"
    exit 1
fi

if [[ ! -d "${flecs_path}" ]]; then
    echo ""
    echo "ERROR: Flecs include directory not found:"
    echo "  ${flecs_path}"
    exit 1
fi

if [[ ! -d "${sdl_path}/include" ]]; then
    echo ""
    echo "ERROR: SDL3 include directory not found:"
    echo "  ${sdl_path}/include"
    exit 1
fi

if [[ ! -f "${sdl_path}/Android.mk" ]]; then
    echo ""
    echo "ERROR: SDL3 Android.mk not found:"
    echo "  ${sdl_path}/Android.mk"
    exit 1
fi

if [[ ! -d "${root}/res" ]]; then
    echo ""
    echo "ERROR: Resource directory not found:"
    echo "  ${root}/res"
    exit 1
fi

echo "- Source tree OK"
echo "- Game tree [gam/${game_name}] OK"

# ============================================================
# SDL3 Android project
# ============================================================

echo ""
echo "> Finding SDL3 Android project"

sdl_android="${sdl_path}/android-project"

if [[ ! -d "${sdl_android}" ]]; then
    echo ""
    echo "ERROR: SDL3 Android project not found:"
    echo "  ${sdl_android}"
    echo ""
    echo "SDL3 must be downloaded with:"
    echo ""
    echo "  bash bsh/libs-download.sh --sdl3"
    echo ""
    exit 1
fi

echo "- Found [${sdl_android}]"

# ============================================================
# Build flags
# ============================================================

cflags="-std=gnu99 -fPIC"
dflags="-Dzox_game=${game_name} -Dflecssource -Dzox_android"
libs="-llog -landroid -lm -lEGL -lGLESv3"
includes="-I${flecs_path} -I${sdl_path}/include"

if [[ "${debug}" == "True" ]]; then
    echo "+ Added [debug]"
    dflags+=" -Dzox_debug"
    cflags+=" -Wall -ggdb3"
    cflags+=" -O0 -g3"
else
    echo "+ Added [release]"
    cflags+=" -O3 -DNDEBUG"
fi

if [[ "${verbose}" == "True" ]]; then
    echo "+ Added [verbose]"
    dflags+=" -Dzox_verbose"
fi

echo "* Added [sdl]"
dflags+=" -Dzox_sdl"

echo "* Added [sdl3]"
dflags+=" -Dzox_sdl3"

echo "* Added [opengl]"
dflags+=" -Dzox_opengl"

# ============================================================
# Build information
# ============================================================

echo ""
echo "Building [${apk_path}]"
echo ""
echo "- Compiler [Android NDK clang]"
echo "- ABI [${android_abi}]"
echo "- API [${android_api}]"
echo "- CFlags [${cflags}]"
echo "- DFlags [${dflags}]"
echo "- Libs [${libs}]"
echo "- Includes [${includes}]"

# ============================================================
# Prepare staging directory
# ============================================================

echo ""
echo "> Preparing [${staging_path}]"

rm -rf "${staging_path}"
mkdir -p "${staging_path}"

# ============================================================
# Copy SDL3 Android project
# ============================================================

cp -a "${sdl_android}/." "${staging_path}/"
echo "- SDL Android project copied"

# ============================================================
# Configure SDL3 native project
# ============================================================

echo ""
echo "> Configuring SDL3 native project"

jni_path="${staging_path}/app/jni"
sdl_jni_path="${jni_path}/SDL"

rm -rf "${sdl_jni_path}"
ln -s "${sdl_path}" "${sdl_jni_path}"

echo "- SDL3 linked"
echo "  ${sdl_jni_path} -> ${sdl_path}"

# ============================================================
# Native source
#
# Copy the whole src/ tree and gam/<game_name>/ so relative
# includes like:
#
#   #include "defines.c"
#   #include "../gam/zoxel/..."
#
# keep working exactly like the desktop build.
# ============================================================

echo ""
echo "> Copying native source tree"

jni_src_path="${jni_path}/src"
jni_gam_path="${jni_path}/gam/${game_name}"
jni_flecs_path="${jni_path}/flecs"

rm -rf "${jni_src_path}"
rm -rf "${jni_path}/gam"
rm -rf "${jni_flecs_path}"

mkdir -p "${jni_src_path}"
mkdir -p "${jni_gam_path}"
mkdir -p "${jni_flecs_path}"

cp -a "${src_path}/." "${jni_src_path}/"
cp -a "${gam_path}/." "${jni_gam_path}/"
cp -a "${flecs_path}/." "${jni_flecs_path}/"

echo "- Source copied"
echo "  ${jni_src_path}"
echo "- Game copied"
echo "  ${jni_gam_path}"
echo "- Flecs copied"
echo "  ${jni_flecs_path}"

# ============================================================
# Native Android.mk
# ============================================================

echo ""
echo "> Creating Android.mk"

cat > "${jni_src_path}/Android.mk" <<EOF
LOCAL_PATH := \$(call my-dir)

include \$(CLEAR_VARS)

LOCAL_MODULE := main

LOCAL_SRC_FILES := \\
    main.c \\
    ../flecs/flecs.c

LOCAL_C_INCLUDES := \\
    \$(LOCAL_PATH) \\
    \$(LOCAL_PATH)/../gam/${game_name} \\
    \$(LOCAL_PATH)/../flecs \\
    \$(LOCAL_PATH)/../SDL/include

LOCAL_CFLAGS := ${cflags} ${dflags}

LOCAL_SHARED_LIBRARIES := SDL3

LOCAL_LDLIBS := ${libs}

include \$(BUILD_SHARED_LIBRARY)
EOF

echo "- Android.mk created"

# ============================================================
# Application.mk
#
# APP_STL is the important fix for:
#
#   undefined symbol: operator new/delete
#
# SDL3's hid.cpp needs the C++ runtime.
# ============================================================

echo ""
echo "> Creating Application.mk"

cat > "${jni_path}/Application.mk" <<EOF
APP_ABI := ${android_abi}
APP_PLATFORM := android-${android_api}
APP_STL := c++_shared
EOF

echo "- Application.mk configured"

# ============================================================
# Resources
# ============================================================

echo ""
echo "> Copying resources"

assets_path="${staging_path}/app/src/main/assets"

rm -rf "${assets_path}"
mkdir -p "${assets_path}"

mkdir -p "${assets_path}/res"
cp -a "${root}/res/." "${assets_path}/res"

echo "- Resources copied"

# ============================================================
# local.properties
# ============================================================

echo ""
echo "> Creating local.properties"

cat > "${staging_path}/local.properties" <<EOF
sdk.dir=${sdk_path}
ndk.dir=${ndk_path}
EOF

# ============================================================
# Gradle JDK
# ============================================================

echo ""
echo "> Configuring Gradle JDK"

gradle_properties="${staging_path}/gradle.properties"

touch "${gradle_properties}"

if grep -q '^org.gradle.java.home=' "${gradle_properties}"; then
    sed -i "s|^org.gradle.java.home=.*|org.gradle.java.home=${java17_path}|" \
        "${gradle_properties}"
else
    printf '\norg.gradle.java.home=%s\n' "${java17_path}" \
        >> "${gradle_properties}"
fi

echo "- Gradle JDK: ${java17_path}"

# ============================================================
# Native build information
# ============================================================

echo ""
echo "> Native build"

echo "> Source:"
echo "> ${src_path}/"

echo ""
echo "> Game:"
echo "> ${gam_path}/"

echo ""
echo "Defines:"
echo "${dflags}"

echo ""
echo "Includes:"
echo "${includes}"

echo ""
echo "Libraries:"
echo "${libs}"

# ============================================================
# Gradle environment
# ============================================================

echo ""
echo "============================================================"
echo "> GRADLE ENVIRONMENT"
echo "============================================================"

echo ""
echo "> Java used by Gradle"

JAVA_HOME="${java17_path}" \
PATH="${java17_path}/bin:${PATH}" \
"${gradle_bin}" --version

# ============================================================
# Gradle build
# ============================================================

echo ""
echo "============================================================"
echo "> GRADLE BUILD"
echo "============================================================"
echo ""

cd "${staging_path}"

if [[ "${debug}" == "True" ]]; then
    echo "> Debug build"

    JAVA_HOME="${java17_path}" \
    PATH="${java17_path}/bin:${PATH}" \
    "${gradle_bin}" \
        --no-daemon \
        assembleDebug

    apk_source="${staging_path}/app/build/outputs/apk/debug/app-debug.apk"
else
    echo "> Release build"

    JAVA_HOME="${java17_path}" \
    PATH="${java17_path}/bin:${PATH}" \
    "${gradle_bin}" \
        --no-daemon \
        assembleRelease

    apk_source="${staging_path}/app/build/outputs/apk/release/app-release.apk"
fi

# ============================================================
# APK
# ============================================================

echo ""
echo "> Packaging APK"

if [[ ! -f "${apk_source}" ]]; then
    echo ""
    echo "ERROR: Gradle completed but APK was not found:"
    echo "  ${apk_source}"
    exit 1
fi

mkdir -p "${root}/bin"

cp -f "${apk_source}" "${apk_path}"

echo ""
echo "+ APK created"
echo "  ${apk_path}"

# ============================================================
# Android device
# ============================================================

if [[ "${install}" == "True" || "${log}" == "True" ]]; then
echo ""
echo "============================================================"
echo "> ANDROID DEVICE"
echo "============================================================"
echo ""

if ! command -v adb >/dev/null 2>&1; then
    echo ""
    echo "ERROR: adb not found."
    echo ""
    echo "Install Android platform-tools or run:"
    echo "  bash bsh/android_prepare.sh"
    exit 1
    fi

    echo "> Checking Android device"

    adb_devices="$(adb devices | awk 'NR > 1 && $2 == "device" {print $1}')"

    if [[ -z "${adb_devices}" ]]; then
        echo ""
        echo "ERROR: No Android device connected."
        echo ""
        echo "Check:"
        echo "  adb devices"
        exit 1
    fi

    echo "- Device:"
    echo "${adb_devices}" | sed 's/^/  /'
fi

# ============================================================
# Install APK
# ============================================================

if [[ "${install}" == "True" ]]; then
    echo ""
    echo "> Installing APK"
    echo ""
    echo "  ${apk_path}"
    adb install -r "${apk_path}"
    echo ""
    echo "+ APK installed"
fi

# ============================================================
# Logcat
# ============================================================

if [[ "${log}" == "True" ]]; then
    echo ""
    echo "============================================================"
    echo "> LOGCAT"
    echo "============================================================"
    echo ""

    aapt_path="${sdk_path}/build-tools/35.0.0/aapt"

    if [[ ! -x "${aapt_path}" ]]; then
        aapt_path="$(find "${sdk_path}/build-tools" -name aapt -type f -executable | sort -V | tail -n 1)"
    fi

    if [[ -z "${aapt_path}" || ! -x "${aapt_path}" ]]; then
        echo "ERROR: aapt not found."
        exit 1
    fi

    package_name="$("${aapt_path}" dump badging "${apk_path}" \
        | sed -n "s/^package: name='\([^']*\)'.*/\1/p" \
        | head -n 1)"

    if [[ -z "${package_name}" ]]; then
        echo "ERROR: Could not determine APK package name."
        exit 1
    fi

    echo "- Package: ${package_name}"

    echo "- Clearing old logcat"
    adb logcat -c || true

    echo "- Starting application"
    adb shell monkey -p "${package_name}" -c android.intent.category.LAUNCHER 1 >/dev/null 2>&1 || true

    pid=""
    for _ in $(seq 1 20); do
        pid="$(adb shell pidof -s "${package_name}" | tr -d '\r')"
        if [[ -n "${pid}" ]]; then
            break
        fi
        sleep 1
    done

    if [[ -z "${pid}" ]]; then
        echo ""
        echo "ERROR: Zoxel process not found:"
        echo "  ${package_name}"
        exit 1
    fi

    echo "- PID: ${pid}"
    echo ""
    echo "Press Ctrl+C to stop logcat."
    echo ""

    # adb logcat --pid="${pid}"
    adb logcat --pid="${pid}" -v threadtime "SDL:V" "*:S"
fi

# ============================================================
# Done
# ============================================================

echo ""
echo "============================================================"
echo "+ Android build complete"
echo "============================================================"
echo ""
echo "Game:   ${game_name}"
echo "ABI:    ${android_abi}"
echo "API:    ${android_api}"
echo "Java:   JDK 17"
echo "Gradle: ${gradle_version}"
echo ""
echo "Source:"
echo "  ${src_path}"
echo ""
echo "Game:"
echo "  ${gam_path}"
echo ""
echo "APK:"
echo "  ${apk_path}"
echo ""
