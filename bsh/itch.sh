#!/usr/bin/env bash
set -euo pipefail

USERNAME="deus0"

# ============================================================
# Itch.io Package Upload
#
# Packages are assigned channels from their filename:
#
# Normal builds:
#   <game>_<platform>_<arch>_...zip
#   <game>_<platform>_<arch>_...apk
#
#   Channel:
#     <platform>_<arch>
#
# Arch/Pacman packages:
#   <game>_<platform>_<arch>_....pkg.tar.zst
#
#   Channel:
#     <platform>_<arch>_arch
#
# Examples:
#
#   zoxel_linux_x64_opengl_sdl_2026_08_13.zip
#       -> linux_x64
#
#   zoxel_linux_x64_opengl_sdl_2026_08_13.pkg.tar.zst
#       -> linux_x64_arch
#
# The "_arch" suffix distinguishes a native Arch/Pacman package
# from the normal Linux build on the same platform/architecture.
# ============================================================

# === Settings ===
package_path="zip"
butler_path="${HOME}/.butler/bin"
butler="${butler_path}/butler"
butler_linux_x86="https://broth.itch.zone/butler/linux-amd64/LATEST/archive/default"
butler_linux_arm="https://broth.itch.zone/butler/linux-arm64/LATEST/archive/default"

# Butler binary must match the machine running this script
case "$(uname -m)" in
    x86_64)
        BUTLER_ZIP_URL="$butler_linux_x86"
        ;;
    aarch64|arm64)
        BUTLER_ZIP_URL="$butler_linux_arm"
        ;;
    *)
        echo "❌ [ERROR] Unsupported architecture: $(uname -m)" >&2
        exit 1
        ;;
esac

# === Butler API key ===
BUTLER_CONFIG_FILE="$HOME/.config/butler/env"
mkdir -p "$(dirname "$BUTLER_CONFIG_FILE")"

if [ -f "$BUTLER_CONFIG_FILE" ]; then
    source "$BUTLER_CONFIG_FILE"
fi

log() {
    echo "🔍 [DEBUG] $*"
}

error_exit() {
    echo "❌ [ERROR] $*" >&2
    exit 1
}

# ============================================================
# Helpers
# ============================================================

get_package_name() {
    local filename="$1"

    case "$filename" in
        *.pkg.tar.zst)
            printf '%s' "${filename%.pkg.tar.zst}"
            ;;
        *)
            printf '%s' "${filename%.*}"
            ;;
    esac
}

get_package_channel() {
    local filename="$1"
    local name
    local game platform arch

    name="$(get_package_name "$filename")"

    IFS='_' read -r game platform arch _ <<< "$name"

    if [[ "$filename" == *.pkg.tar.zst ]]; then
        printf '%s_%s_arch' "$platform" "$arch"
    else
        printf '%s_%s' "$platform" "$arch"
    fi
}

# === Check package directory ===
if [ ! -d "$package_path" ]; then
    error_exit "Package directory not found: $package_path"
fi

# === Find/install Butler ===
if [ -f "$butler" ]; then
    log "Found Butler Binary"
else
    TMP_DIR=$(mktemp -d)

    log "Downloading and installing Butler"

    curl -sSLf "$BUTLER_ZIP_URL" -o "$TMP_DIR/butler.zip" \
        && unzip -q "$TMP_DIR/butler.zip" -d "$TMP_DIR" \
        && mkdir -p "$butler_path" \
        && mv "$TMP_DIR/butler" "$butler" \
        && chmod +x "$butler" \
        || {
            rm -rf "$TMP_DIR"
            error_exit "Butler install failed"
        }

    rm -rf "$TMP_DIR"
    log "Butler installed successfully."
fi

# === Butler API key ===
if [ -z "${BUTLER_API_KEY-}" ]; then
    echo "Setting Butler API Key"
    echo "Paste API Key (https://itch.io/user/settings/api-key):"
    read -r INPUT_KEY

    INPUT_KEY="${INPUT_KEY//[[:space:]]/}"

    if [ -n "$INPUT_KEY" ]; then
        log "Saving Butler API key to config"

        echo "export BUTLER_API_KEY=\"$INPUT_KEY\"" > "$BUTLER_CONFIG_FILE"

        export BUTLER_API_KEY="$INPUT_KEY"
    else
        error_exit "No API key entered. Exiting."
    fi
fi

if [ -z "${BUTLER_API_KEY-}" ]; then
    error_exit "BUTLER_API_KEY not set."
fi

# ============================================================
# Scan packages
#
# Expected:
#
# <game>_<platform>_<arch>_others.zip
# <game>_<platform>_<arch>_others.apk
#
# Channel:
#
# <platform>_<arch>
#
# Example:
#
# zoxel_linux_x64_opengl_sdl_2026_08_12.zip
#       ^      ^
#       |      |
#    platform arch
#
# Channel = linux_x64
# ============================================================

games=()
channels=()

while IFS= read -r -d '' package_file; do
    filename="$(basename "$package_file")"
    name="$(get_package_name "$filename")"

    IFS='_' read -r game platform arch _ <<< "$name"

    if [ -z "${game:-}" ] || [ -z "${platform:-}" ] || [ -z "${arch:-}" ]; then
        continue
    fi

    # Add game if not already present
    if [[ ! " ${games[*]} " =~ " ${game} " ]]; then
        games+=("$game")
    fi

done < <(
    find "$package_path" -maxdepth 1 -type f \
        \( -name '*.zip' -o -name '*.apk' -o -name '*.pkg.tar.zst' \) \
        -print0
)

if [ "${#games[@]}" -eq 0 ]; then
    error_exit "No valid packages found in $package_path"
fi

# === Pick game ===
echo
echo "🎮 Select game:"
echo

select GAME in "${games[@]}"; do
    if [ -n "${GAME:-}" ]; then
        break
    fi

    echo "❌ Invalid selection."
done

# === Find channels for selected game ===
channels=()

while IFS= read -r -d '' package_file; do
    filename="$(basename "$package_file")"
    name="$(get_package_name "$filename")"

    IFS='_' read -r game platform arch _ <<< "$name"

    if [ "$game" != "$GAME" ]; then
        continue
    fi

    channel="$(get_package_channel "$filename")"

    if [[ ! " ${channels[*]} " =~ " ${channel} " ]]; then
        channels+=("$channel")
    fi

done < <(
    find "$package_path" -maxdepth 1 -type f \
        \( -name '*.zip' -o -name '*.apk' -o -name '*.pkg.tar.zst' \) \
        -print0
)

if [ "${#channels[@]}" -eq 0 ]; then
    error_exit "No channels found for game: $GAME"
fi

# === Pick channel ===
echo
echo "📦 Select channel for $GAME:"
echo

select CHANNEL in "${channels[@]}"; do
    if [ -n "${CHANNEL:-}" ]; then
        break
    fi

    echo "❌ Invalid selection."
done

# ============================================================
# Find newest package matching game + channel
# ============================================================

PACKAGE_NAME=""

while IFS= read -r -d '' package_file; do
    filename="$(basename "$package_file")"

    name="$(get_package_name "$filename")"
    IFS='_' read -r file_game platform arch _ <<< "$name"

    if [[ "$file_game" != "$GAME" ]]; then
        continue
    fi

    file_channel="$(get_package_channel "$filename")"

    if [[ "$file_channel" != "$CHANNEL" ]]; then
        continue
    fi

    if [[ -z "$PACKAGE_NAME" || "$package_file" -nt "$PACKAGE_NAME" ]]; then
        PACKAGE_NAME="$package_file"
    fi

done < <(
    find "$package_path" -maxdepth 1 -type f \
        \( -name '*.zip' -o -name '*.apk' -o -name '*.pkg.tar.zst' \) \
        -print0
)

if [[ -z "$PACKAGE_NAME" ]]; then
    error_exit "No package found for ${GAME}:${CHANNEL}"
fi

case "$PACKAGE_NAME" in
    *.pkg.tar.zst)
        PACKAGE_TYPE="Arch/Pacman"
        ;;
    *.apk)
        PACKAGE_TYPE="Android"
        ;;
    *.zip)
        PACKAGE_TYPE="Archive"
        ;;
    *)
        PACKAGE_TYPE="Unknown"
        ;;
esac

echo
echo "🎮 Game:    ${GAME}"
echo "📦 Channel: ${CHANNEL}"
echo "📦 Type:    ${PACKAGE_TYPE}"
echo "📁 Package: ${PACKAGE_NAME}"
echo

echo "🚀 Uploading ${PACKAGE_NAME} to ${USERNAME}/${GAME}:${CHANNEL} ..."

"${butler}" push "${PACKAGE_NAME}" "${USERNAME}/${GAME}:${CHANNEL}"

echo "✅ Upload complete."