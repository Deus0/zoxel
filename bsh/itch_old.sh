#!/usr/bin/env bash
set -euo pipefail

# Example Call: ./bsh/itch.sh "zoxel" "linux_arm" "bin/zoxel_linux_arm_opengl_sdl_2026_06_01.zip"

USERNAME="deus0"    # Your itch.io username
GAME="${1}"         # Your itch.io project
CHANNEL="${2}"      # The itch io channel: zoxel-windows etc
ZIP_NAME="${3}"     # The file to upload to itch io
# === Settings ===
butler_path="${HOME}/.butler/bin"
butler="${butler_path}/butler"
PROFILE_SCRIPT="${HOME}/.bashrc"  # or ~/.zshrc if you use zsh
# update with url of butler
butler_linux_x86="https://broth.itch.zone/butler/linux-amd64/LATEST/archive/default"
butler_linux_arm="https://broth.itch.zone/butler/linux-arm64/LATEST/archive/default"
BUTLER_ZIP_URL="${butler_linux_arm}"
# env key
BUTLER_CONFIG_FILE="$HOME/.config/butler/env"
mkdir -p "$(dirname "$BUTLER_CONFIG_FILE")"

if [ -f "$BUTLER_CONFIG_FILE" ]; then
    source "$BUTLER_CONFIG_FILE"
fi

log() { echo "🔍 [DEBUG] $*"; }
error_exit() { echo "❌ [ERROR] $*" >&2; exit 1; }

if [ -f "${butler}" ]; then
    log "+ Found Butler Binary"
else
    TMP_DIR=$(mktemp -d)
    log "Downloading and installing butler"
    curl -sSLf "$BUTLER_ZIP_URL" -o "$TMP_DIR/butler.zip" \
        && unzip -q "$TMP_DIR/butler.zip" -d "$TMP_DIR" \
        && mkdir -p "$butler_path" \
        && mv "$TMP_DIR/butler" "${butler}" \
        && chmod +x "${butler}" \
        || { rm -rf "$TMP_DIR"; error_exit "Butler install failed"; }
    rm -rf "$TMP_DIR"
    log "Butler installed successfully."
fi

if [ -z "${butler}" ]; then
    echo "❌ Butler is STILL not installed at [${butler}]"
    exit 1
fi

# Setup API Key
if [ -n "${BUTLER_API_KEY-}" ]; then
    log "+ Found Butler API Key"
else
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

# === ENVIRONMENT CHECK ===

if [ -z "${BUTLER_API_KEY-}" ]; then
    echo "❌ BUTLER_API_KEY not set. Run: export BUTLER_API_KEY='your_api_key'"
    exit 1
fi

if [ -z "$ZIP_NAME" ]; then
    echo "❌ No Zip [${ZIP_NAME}] Found"
    exit
fi

echo "🚀 Uploading ${ZIP_NAME} to ${USERNAME}/${GAME}:${CHANNEL} ..."
${butler} push "${ZIP_NAME}" "${USERNAME}/${GAME}:${CHANNEL}"
echo "✅ Upload complete."
