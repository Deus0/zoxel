#!/usr/bin/env bash
set -euo pipefail

# NOTE: --signed automatically creates new keys

# Test
#    bash bsh/android.sh zoxel --opengl --sdl --debug --install
# Debug
#    bash bsh/android.sh zoxel opengl sdl --debug --install --logs --verbose
# Debug
# bash bsh/android.sh zoxel --debug --install
# Optimized release, unsigned
# bash bsh/android.sh zoxel --release --unsigned
# Optimized release, signed
# bash bsh/android.sh zoxel --release --signed
# Optimized release, signed, install
# bash bsh/android.sh zoxel --release --signed --install
# XR release, signed, install
# bash bsh/android.sh zoxel --release --signed --xr --install

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

# ============================================================
# Script Settings
# ============================================================

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
host_arch="$(uname -m)"
apk_dir="${root}/zip"           # Output Directory
game_name="${1:-zoxel}"
app_name="${game_name^}"    # First letter will be capitalized

is_run="0"
sdl_mixer="1"
profile="debug"
signed="0"
install="0"
logcat="0"
logs="0"
verbose="0"
gfx="opengl"    # opengl, vulkan, headless
is_xr="0"
testsdl="0"
testxr="0"

# set directory
cd "${root}"

# Validation
if [[ " $* " == *" --debug "* ]] &&
   [[ " $* " == *" --release "* ]]; then
    echo "ERROR: --debug and --release cannot be used together."
    exit 1
fi
if [[ " $* " == *" --unsigned "* ]] &&
   [[ " $* " == *" --signed "* ]]; then
    echo "ERROR: --signed and --unsigned cannot be used together."
    exit 1
fi

# Parse arguments
[[ " $* " == *" --debug "* ]] && profile="debug"
[[ " $* " == *" --release "* ]] && profile="release"
[[ " $* " == *" --unsigned "* ]] && signed="0"
[[ " $* " == *" --signed "* ]] && signed="1"
[[ " $* " == *" --install "* ]] && install="1"
[[ " $* " == *" --run "* ]] && is_run="1"
[[ " $* " == *" --logcat "* ]] && logcat="1"
[[ " $* " == *" --logs "* ]] && logs="1"
[[ " $* " == *" --verbose "* ]] && verbose="1"
[[ " $* " == *" --nomixer "* ]] && sdl_mixer="0"
[[ " $* " == *" --opengl "* ]] && gfx="opengl"
[[ " $* " == *" --vulkan "* ]] && gfx="vulkan"
[[ " $* " == *" --headless "* ]] && gfx="headless"
[[ " $* " == *" --xr "* ]] && is_xr="1"
[[ " $* " == *" --testxr "* ]] && testxr="1"
[[ " $* " == *" --testsdl "* ]] && testsdl="1"

date_str=$(date +%Y_%m_%d)

apk_path="${apk_dir}/${game_name}_android_arm"
[[ "${is_xr}" == "1" ]] && apk_path="${apk_path}_xr"
apk_path="${apk_path}_${date_str}.apk"

# make our export folder
mkdir -p "${apk_dir}"

# ============================================================
# Pathing
# ============================================================

android_abi="arm64-v8a"
and_path="${root}/and"
sdk_path="${and_path}/sdk"
ndk_path="${and_path}/ndk"
staging_path="${and_path}/${game_name}"
aapt2_path="${and_path}/tools/aapt2"
jni_path="${staging_path}/app/jni"
sdl_jni_path="${jni_path}/SDL"
src_path="${root}/src"
gam_path="${root}/gam/${game_name}"
flecs_path="${root}/inc/flecs"
sdl_path="${root}/ext/sdl3"
sdl_mixer_path="${root}/ext/sdl3_mixer"
local_main_file="main.c"

[[  "${testsdl}" == "1" ]] && local_main_file="apps/sdl/tst/app.c"
[[  "${testxr}" == "1" ]] && local_main_file="xr/tst/cube.c"
main_file="${src_path}/${local_main_file}" # "${src_path}/main.c"

sources=(
    "${main_file}"
    "${flecs_path}/flecs.c"
)

# ============================================================
# Android configuration
# ============================================================

android_api="35"
gradle_path="${and_path}/gradle"
gradle_version="8.10.2"
strings_path="${staging_path}/app/src/main/res/values/strings.xml"

# ============================================================
# SDL3_mixer configuration
# ============================================================

sdl_mixer_flags=(
    "SUPPORT_MOD_XMP=false"
    "SUPPORT_WAVPACK=false"
    "SUPPORT_OPUS=false"
    "SUPPORT_FLAC_LIBFLAC=false"
    "SUPPORT_FLAC_DRFLAC=true"
)


# ============================================================
# JDK 17
# ============================================================

java17_path=""

# Prefer the currently installed Java if it is JDK 17.

if command -v java >/dev/null 2>&1; then
    java_bin="$(readlink -f "$(command -v java)")"

    java_major="$("${java_bin}" -version 2>&1 | \
        sed -n 's/.*version "\([0-9]*\).*/\1/p' | head -n 1)"

    if [[ "${java_major}" == "17" ]]; then
        java17_path="$(dirname "$(dirname "${java_bin}")")"
    fi
fi

# Fallback to known JDK 17 locations.

if [[ -z "${java17_path}" ]]; then
    for candidate in \
        "/usr/lib/jvm/java-17-openjdk" \
        "/usr/lib/jvm/java-17-openjdk-arm64"
    do
        if [[ -x "${candidate}/bin/java" ]] && \
           [[ -x "${candidate}/bin/javac" ]]; then
            java17_path="${candidate}"
            break
        fi
    done
fi

if [[ -z "${java17_path}" ]]; then
    echo ""
    echo "ERROR: JDK 17 not found."
    echo ""
    echo "Run:"
    echo "  bash bsh/android_prepare.sh"
    echo ""
    exit 1
fi

if [[ ! -x "${java17_path}/bin/java" ]]; then
    echo ""
    echo "ERROR: JDK 17 java not found:"
    echo "  ${java17_path}/bin/java"
    exit 1
fi

if [[ ! -x "${java17_path}/bin/javac" ]]; then
    echo ""
    echo "ERROR: JDK 17 javac not found:"
    echo "  ${java17_path}/bin/javac"
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

# Validate SDL_Mixer

if [[ "${sdl_mixer}" == "1" ]]; then
    if [[ ! -d "${sdl_mixer_path}" ]]; then
        echo ""
        echo "ERROR: SDL3_mixer directory not found:"
        echo "  ${sdl_mixer_path}"
        exit 1
    fi

    if [[ ! -f "${sdl_mixer_path}/Android.mk" ]]; then
        echo ""
        echo "ERROR: SDL3_mixer Android.mk not found:"
        echo "  ${sdl_mixer_path}/Android.mk"
        exit 1
    fi

    if [[ ! -d "${sdl_mixer_path}/include" ]]; then
        echo ""
        echo "ERROR: SDL3_mixer include directory not found:"
        echo "  ${sdl_mixer_path}/include"
        exit 1
    fi
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
dflags="-Dzox_game=${game_name} -Dflecssource -Dzox_android -Dzox_safety_checks"
libs="-llog -landroid -lm -lEGL -lGLESv3"
includes="-I${flecs_path} -I${sdl_path}/include"

if [[ "${profile}" == "debug" ]]; then
    echo "+ Added [zox_debug]"
    dflags+=" -Dzox_debug"
    cflags+=" -Wall -ggdb3"
    cflags+=" -O0 -g3"
else
    echo "+ Added [release]"
    cflags+=" -O3 -DNDEBUG"
fi


if [[ "${logs}" == "1" ]]; then
    echo "+ Added [zox_logs]"
    dflags+=" -Dzox_logs"
fi

if [[ "${verbose}" == "1" ]]; then
    echo "+ Added [verbose]"
    dflags+=" -Dzox_verbose"
fi

echo "* Added [sdl]"
dflags+=" -Dzox_sdl"

echo "* Added [sdl3]"
dflags+=" -Dzox_sdl3"

if [[ "${gfx}" == "opengl" ]]; then
    echo "* Added [opengl]"
    dflags+=" -Dzox_opengl"
fi

if [[ "${sdl_mixer}" == "1" ]]; then
    echo "* Added [sdl_mixer]"
    dflags+=" -Dzox_sdl_mixer"
    includes+=" -I${sdl_mixer_path}/include"
else
    echo "* SDL3_mixer disabled"
fi

if [[ "${is_xr}" == "1" ]]; then
    echo "* Added [xr]"
    dflags+=" -Dzox_xr"
    dflags+=" -DXR_USE_PLATFORM_ANDROID"
    dflags+=" -DXR_USE_GRAPHICS_API_OPENGL_ES"
fi

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
# Copy SDL3 Android Project
# ============================================================

cp -a "${sdl_android}/." "${staging_path}/"
echo "- SDL Android project copied"


# ============================================================
# OpenXR
# ============================================================

if [[ "${is_xr}" == "1" ]]; then
    echo ""
    echo "> Preparing OpenXR"
    bash "${root}/bsh/xr.sh" "${staging_path}"
    echo "- OpenXR prepared"
fi

# ============================================================
# Application name
# ============================================================


# Edit our games name
sed -i \
    "s|<string name=\"app_name\">.*</string>|<string name=\"app_name\">${app_name}</string>|" \
    "${strings_path}"

echo "- App name [${app_name}]"

# ============================================================
# Configure SDL3 native project
# ============================================================

echo ""
echo "> Configuring SDL3 native project"

rm -rf "${sdl_jni_path}"
ln -s "${sdl_path}" "${sdl_jni_path}"
echo "- SDL3 linked"
echo "  ${sdl_jni_path} -> ${sdl_path}"

if [[ ${sdl_mixer} == "1" ]]; then
    sdl_mixer_jni_path="${jni_path}/SDL_mixer"
    rm -rf "${sdl_mixer_jni_path}"
    ln -s "${sdl_mixer_path}" "${sdl_mixer_jni_path}"
    echo "- SDL_mixer linked"
    echo "  ${sdl_mixer_jni_path} -> ${sdl_mixer_path}"
fi

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
# Verification
# ============================================================

echo ""
echo "> Verifying staging paths"

echo "- Main Source:"
ls -l "${jni_src_path}/${local_main_file}"

echo "- SDL:"
ls -ld "${sdl_jni_path}"
ls -ld "${sdl_jni_path}/include"

echo "- Flecs:"
ls -l "${jni_flecs_path}/flecs.c"

if [[ "${is_xr}" == "1" ]]; then
    echo "- OpenXR include:"
    ls -ld "${jni_path}/xr/include"

    echo "- OpenXR loader:"
    ls -l "${jni_path}/xr/android/${android_abi}/libopenxr_loader.so"
fi

echo ""
echo "- Android.mk:"
sed 's/^/  /' "${jni_src_path}/Android.mk" 2>/dev/null || true


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
${local_main_file} \\
../flecs/flecs.c

LOCAL_C_INCLUDES := \\
\$(LOCAL_PATH) \\
\$(LOCAL_PATH)/../gam/${game_name} \\
\$(LOCAL_PATH)/../flecs \\
\$(LOCAL_PATH)/../SDL/include
EOF

if [[ "${sdl_mixer}" == "1" ]]; then
cat >> "${jni_src_path}/Android.mk" <<EOF
LOCAL_C_INCLUDES += \\
\$(LOCAL_PATH)/../SDL_mixer/include

EOF
fi

if [[ "${is_xr}" == "1" ]]; then
cat >> "${jni_src_path}/Android.mk" <<EOF
LOCAL_C_INCLUDES += \\
\$(LOCAL_PATH)/../xr/include

EOF
fi

cat >> "${jni_src_path}/Android.mk" <<EOF
LOCAL_CFLAGS := ${cflags} ${dflags}

LOCAL_SHARED_LIBRARIES := SDL3
EOF

if [[ "${sdl_mixer}" == "1" ]]; then
cat >> "${jni_src_path}/Android.mk" <<EOF
LOCAL_SHARED_LIBRARIES += SDL3_mixer
EOF
fi

if [[ "${is_xr}" == "1" ]]; then
cat >> "${jni_src_path}/Android.mk" <<EOF
LOCAL_SHARED_LIBRARIES += openxr_loader
EOF
fi

cat >> "${jni_src_path}/Android.mk" <<EOF

LOCAL_LDLIBS := ${libs}

include \$(BUILD_SHARED_LIBRARY)
EOF

# ------------------------------------------------------------
# OpenXR prebuilt loader
# ------------------------------------------------------------

if [[ "${is_xr}" == "1" ]]; then
cat >> "${jni_src_path}/Android.mk" <<EOF

include \$(CLEAR_VARS)

LOCAL_MODULE := openxr_loader
LOCAL_SRC_FILES := ../xr/android/arm64-v8a/libopenxr_loader.so

include \$(PREBUILT_SHARED_LIBRARY)
EOF
fi

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

$(printf '%s\n' "${sdl_mixer_flags[@]}")
EOF

echo "- Application.mk configured"

# ============================================================
# Android minimum SDK
# ============================================================

app_gradle="${staging_path}/app/build.gradle"

if [[ "${is_xr}" == "1" ]]; then
    android_min_api="24"
else
    android_min_api="21"
fi

echo "- Minimum Android API [${android_min_api}]"

sed -i \
    "s/minSdkVersion [0-9]*/minSdkVersion ${android_min_api}/" \
    "${app_gradle}"

# ============================================================
# Application ID / namespace
# ============================================================

# package_name="org.zox.${game_name}"
package_name="org.libsdl.app"

if [[ "${profile}" == "debug" ]]; then
    package_name="${package_name}.debug"
fi

echo "- Application ID [${package_name}]"

#sed -i \
#    "s|namespace[[:space:]]*=.*|namespace = \"${package_name}\"|" \
#    "${app_gradle}"

#sed -i \
#    "/defaultConfig[[:space:]]*{/a\\
#        applicationId \"${package_name}\"" \
#    "${app_gradle}"

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

if [[ "${host_arch}" == "aarch64" ]]; then
    if [[ ! -x "${aapt2_path}" ]]; then
        echo ""
        echo "ERROR: ARM64 AAPT2 not found:"
        echo "  ${aapt2_path}"
        echo ""
        echo "Run:"
        echo "  bash bsh/android_prepare.sh"
        exit 1
    fi
    if grep -q '^android.aapt2FromMavenOverride=' "${gradle_properties}"; then
        sed -i \
            "s|^android.aapt2FromMavenOverride=.*|android.aapt2FromMavenOverride=${aapt2_path}|" \
            "${gradle_properties}"
    else
        printf 'android.aapt2FromMavenOverride=%s\n' \
            "${aapt2_path}" \
            >> "${gradle_properties}"
    fi

    echo "- ARM64 AAPT2: ${aapt2_path}"
fi

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
# Release signing
# ============================================================

keystore_dir="${HOME}/.config/zox_keys"
keystore_path="${keystore_dir}/${game_name}.keystore"
keystore_password_file="${keystore_dir}/${game_name}.password"
keystore_alias="${game_name}"

if [[ "${profile}" == "release" && "${signed}" == "1" ]]; then

    echo ""
    echo "> Configuring release signing"

    mkdir -p "${keystore_dir}"
    chmod 700 "${keystore_dir}"

    # --------------------------------------------------------
    # Password
    # --------------------------------------------------------

    if [[ ! -f "${keystore_password_file}" ]]; then
        echo "- Creating keystore password"

        keystore_password="$(head -c 32 /dev/urandom | base64)"

        printf '%s\n' "${keystore_password}" \
            > "${keystore_password_file}"

        chmod 600 "${keystore_password_file}"

        echo "- Password created:"
        echo "  ${keystore_password_file}"
    else
        keystore_password="$(<"${keystore_password_file}")"
    fi

    if [[ -z "${keystore_password}" ]]; then
        echo ""
        echo "ERROR: Keystore password file is empty:"
        echo "  ${keystore_password_file}"
        exit 1
    fi

    # --------------------------------------------------------
    # Keystore
    # --------------------------------------------------------

    if [[ ! -f "${keystore_path}" ]]; then

        echo ""
        echo "- Release keystore not found"
        echo "- Creating:"
        echo "  ${keystore_path}"

        keytool \
            -genkeypair \
            -v \
            -keystore "${keystore_path}" \
            -storepass "${keystore_password}" \
            -keypass "${keystore_password}" \
            -alias "${keystore_alias}" \
            -keyalg RSA \
            -keysize 2048 \
            -validity 10000 \
            -dname "CN=${app_name}"

        chmod 600 "${keystore_path}"

        echo "+ Release keystore created"

    else

        echo "- Release keystore found:"
        echo "  ${keystore_path}"

    fi

    # --------------------------------------------------------
    # Gradle signing configuration
    # --------------------------------------------------------

    app_gradle="${staging_path}/app/build.gradle"

    echo "- Configuring Gradle signing"
    echo "  ${keystore_path}"

    cat >> "${app_gradle}" <<EOF

android {
    signingConfigs {
        release {
            storeFile file("${keystore_path}")
            storePassword "${keystore_password}"
            keyAlias "${keystore_alias}"
            keyPassword "${keystore_password}"
        }
    }

    buildTypes {
        release {
            signingConfig signingConfigs.release
        }
    }
}
EOF

    echo "+ Release signing configured"
fi


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

if [[ "${profile}" == "debug" ]]; then
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

    # apk_source="${staging_path}/app/build/outputs/apk/release/app-release.apk"

    release_dir="${staging_path}/app/build/outputs/apk/release"
    apk_source="${release_dir}/app-release.apk"

    if [[ ! -f "${apk_source}" ]]; then
        apk_source="${release_dir}/app-release-unsigned.apk"
    fi

    if [[ ! -f "${apk_source}" ]]; then
        echo ""
        echo "ERROR: Gradle completed but APK was not found:"
        echo "  ${release_dir}"
        find "${release_dir}" -maxdepth 1 -type f -name "*.apk" -print
        exit 1
    fi
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

if [[ "${install}" == "1" || "${logcat}" == "1" ]]; then

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

if [[ "${install}" == "1" ]]; then
    echo ""
    echo "> Installing APK"
    echo ""
    echo "  ${apk_path}"
    adb install -r "${apk_path}"
    echo ""
    echo "+ APK installed"
fi

# ============================================================
# Running
# ============================================================

if [[ "${is_run}" == "1" ]]; then
    echo ""
    echo "============================================================"
    echo "> Running"
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

    echo "- Stopping previous application"
    adb shell am force-stop "${package_name}" || true

    sleep 2

    echo "- Starting application"
    adb shell monkey -p "${package_name}" \
        -c android.intent.category.LAUNCHER 1
            sleep 1

    if [[ "${logcat}" == "1" ]]; then
        if [[ "${is_xr}" == "1" ]]; then
            echo "Delaying for Logcat..."
            sleep 15
        fi
        echo "============================================================"
        echo "> LOGCAT"
        echo "============================================================"
        echo ""
        adb logcat -d -v threadtime "Zoxel:I" "*:S"
    fi
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
