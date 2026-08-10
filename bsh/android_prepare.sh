#!/usr/bin/env bash
set -euo pipefail

# ============================================================
# Zoxel Android Environment Preparation
#
# Creates:
#
# and/
# ├── sdk/
# ├── ndk/
# └── gradle/
#
# Does NOT create:
#
# and/zoxel/
#
# That is created by android.sh.
#
# Gradle is deliberately run with JDK 17.
# Newer system JDKs can be too new for the Gradle/Groovy
# version used by this Android build.
# ============================================================

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

and_path="${root}/and"
sdk_path="${and_path}/sdk"
ndk_path="${and_path}/ndk"
gradle_path="${and_path}/gradle"

# Android versions
android_api="35"
android_build_tools="35.0.0"
android_ndk_version="28.2.13676358"

# Gradle
gradle_version="8.10.2"

# Java
java_major_required="17"

# Google command-line tools
cmdline_tools_version="11076708"
cmdline_tools_zip="commandlinetools-linux-${cmdline_tools_version}_latest.zip"
cmdline_tools_url="https://dl.google.com/android/repository/${cmdline_tools_zip}"

gradle_zip="gradle-${gradle_version}-bin.zip"
gradle_url="https://services.gradle.org/distributions/${gradle_zip}"

tmp_path="${and_path}/tmp"

echo ""
echo "> Preparing Android build environment"
echo ""

# ============================================================
# Basic tools
# ============================================================

if ! command -v curl >/dev/null 2>&1; then
    echo "ERROR: curl is required."
    echo ""
    echo "Install curl with:"
    echo "  sudo pacman -S curl"
    exit 1
fi

if ! command -v unzip >/dev/null 2>&1; then
    echo "ERROR: unzip is required."
    echo ""
    echo "Install unzip with:"
    echo "  sudo pacman -S unzip"
    exit 1
fi

# ============================================================
# Java / JDK 17
#
# Do NOT use the generic jdk-openjdk package here.
# On current CachyOS/Arch this can install a much newer JDK.
#
# We specifically want JDK 17 for Gradle.
# ============================================================

java17_path=""

# First look for an already installed Java 17.

for candidate in \
    "/usr/lib/jvm/java-17-openjdk" \
    "/usr/lib/jvm/java-17-openjdk/bin/java"
do
    if [[ -x "$candidate" ]]; then
        if [[ -x "$candidate/bin/java" ]]; then
            java17_path="$candidate"
        elif [[ "$(basename "$candidate")" == "java" ]]; then
            java17_path="$(dirname "$(dirname "$candidate")")"
        fi
        break
    fi
done

# If Java 17 was not found, install it.

if [[ -z "${java17_path}" ]]; then
    echo "> Java 17 not found"
    echo "> Installing JDK 17"

    if command -v pacman >/dev/null 2>&1; then
        sudo pacman -S --needed jdk17-openjdk
    else
        echo ""
        echo "ERROR: pacman is not available."
        echo "Please install a JDK 17 manually."
        exit 1
    fi

    java17_path="/usr/lib/jvm/java-17-openjdk"
fi

if [[ ! -x "${java17_path}/bin/java" ]]; then
    echo ""
    echo "ERROR: JDK 17 was not found at:"
    echo "  ${java17_path}"
    exit 1
fi

if [[ ! -x "${java17_path}/bin/javac" ]]; then
    echo ""
    echo "ERROR: javac was not found in JDK 17."
    exit 1
fi

# Use JDK 17 for everything this script launches.

export JAVA_HOME="${java17_path}"
export PATH="${JAVA_HOME}/bin:${PATH}"

echo ""
echo "> Java"
echo "  JAVA_HOME=${JAVA_HOME}"
"${JAVA_HOME}/bin/java" -version
echo ""

# Verify the actual major version.

java_major="$("${JAVA_HOME}/bin/java" -version 2>&1 | \
    sed -n 's/.*version "\([0-9]*\).*/\1/p' | head -n 1)"

if [[ "${java_major}" != "${java_major_required}" ]]; then
    echo ""
    echo "ERROR: Android build requires JDK ${java_major_required}."
    echo "Found JDK ${java_major} instead."
    echo ""
    echo "JAVA_HOME=${JAVA_HOME}"
    exit 1
fi

echo "+ JDK ${java_major_required} selected"

# ============================================================
# Create directories
# ============================================================

echo ""
echo "> Creating Android directories"

mkdir -p "${and_path}"
mkdir -p "${sdk_path}"
mkdir -p "${ndk_path}"
mkdir -p "${gradle_path}"
mkdir -p "${tmp_path}"

# ============================================================
# Android command-line tools
# ============================================================

sdkmanager_path="${sdk_path}/cmdline-tools/latest/bin/sdkmanager"

if [[ ! -x "${sdkmanager_path}" ]]; then
    echo "> Downloading Android command-line tools"
    echo "  ${cmdline_tools_zip}"

    cmdline_zip="${tmp_path}/${cmdline_tools_zip}"

    curl -L \
        --fail \
        --progress-bar \
        "${cmdline_tools_url}" \
        -o "${cmdline_zip}"

    echo "> Installing Android command-line tools"

    rm -rf "${sdk_path}/cmdline-tools"

    mkdir -p "${sdk_path}/cmdline-tools"

    unzip -q \
        "${cmdline_zip}" \
        -d "${sdk_path}/cmdline-tools"

    mv \
        "${sdk_path}/cmdline-tools/cmdline-tools" \
        "${sdk_path}/cmdline-tools/latest"
else
    echo "+ Android command-line tools already installed"
fi

# ============================================================
# Android SDK environment
# ============================================================

export ANDROID_HOME="${sdk_path}"
export ANDROID_SDK_ROOT="${sdk_path}"

export PATH="${sdk_path}/cmdline-tools/latest/bin:${sdk_path}/platform-tools:${PATH}"

echo ""
echo "> Android SDK"
echo "  ${ANDROID_HOME}"
echo ""

# ============================================================
# Accept licenses
# ============================================================

echo "> Accepting Android SDK licenses"

yes | "${sdkmanager_path}" --licenses >/dev/null || true

# ============================================================
# Install Android SDK components
# ============================================================

echo "> Installing Android SDK components"

"${sdkmanager_path}" \
    "platform-tools" \
    "platforms;android-${android_api}" \
    "build-tools;${android_build_tools}"

# ============================================================
# Install NDK
# ============================================================

ndk_manager_path="${sdk_path}/ndk/${android_ndk_version}"

if [[ ! -d "${ndk_manager_path}" ]]; then
    echo "> Installing Android NDK ${android_ndk_version}"

    "${sdkmanager_path}" \
        "ndk;${android_ndk_version}"
else
    echo "+ Android NDK already installed"
fi

# Stable project-local NDK path.
#
# android.sh can always use:
#
# and/ndk
#
# regardless of the actual NDK version.

rm -rf "${ndk_path}"

ln -s \
    "${ndk_manager_path}" \
    "${ndk_path}"

export ANDROID_NDK_HOME="${ndk_manager_path}"

echo ""
echo "> Android NDK"
echo "  ${ANDROID_NDK_HOME}"
echo ""

# ============================================================
# Gradle
# ============================================================

gradle_bin="${gradle_path}/gradle-${gradle_version}/bin/gradle"

if [[ ! -x "${gradle_bin}" ]]; then
    echo "> Downloading Gradle ${gradle_version}"

    gradle_zip_path="${tmp_path}/${gradle_zip}"

    curl -L \
        --fail \
        --progress-bar \
        "${gradle_url}" \
        -o "${gradle_zip_path}"

    echo "> Installing Gradle"

    rm -rf "${gradle_path}/gradle-${gradle_version}"

    unzip -q \
        "${gradle_zip_path}" \
        -d "${gradle_path}"
else
    echo "+ Gradle ${gradle_version} already installed"
fi

# ============================================================
# Test Gradle
#
# Important:
# Explicitly use JDK 17 here. This catches the exact class
# version problem before android.sh gets involved.
# ============================================================

echo ""
echo "> Testing Gradle"
echo ""

JAVA_HOME="${java17_path}" \
PATH="${java17_path}/bin:${PATH}" \
"${gradle_bin}" --version

# ============================================================
# Test SDK
# ============================================================

echo ""
echo "> Testing Android SDK"
echo ""

"${sdkmanager_path}" --version

# ============================================================
# Test NDK
# ============================================================

echo ""
echo "> Testing Android NDK"
echo ""

if [[ ! -x "${ANDROID_NDK_HOME}/ndk-build" ]]; then
    echo "ERROR: NDK installation appears incomplete."
    exit 1
fi

"${ANDROID_NDK_HOME}/ndk-build" --version || true

# ============================================================
# Cleanup
# ============================================================

rm -rf "${tmp_path}"

echo ""
echo "+ Android environment ready"
echo ""
echo "  Java:   ${JAVA_HOME}"
echo "  SDK:    ${sdk_path}"
echo "  NDK:    ${ndk_path}"
echo "  Gradle: ${gradle_bin}"
echo ""
echo "Run:"
echo ""
echo "  bash bsh/android.sh"
echo ""