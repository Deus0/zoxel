#!/usr/bin/env bash
set -euo pipefail

# Example:
# ./bsh/itch_latest.sh "zoxel" "linux_arm"

USERNAME="deus0"
GAME="${1:-}"
CHANNEL="${2:-}"

# === Settings ===
package_path="zip"
butler_path="${HOME}/.butler/bin"
butler="${butler_path}/butler"
PROFILE_SCRIPT="${HOME}/.bashrc"
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

if [ -z "$GAME" ] || [ -z "$CHANNEL" ]; then
    echo "Usage: $0 <game> <channel>"
    exit 1
fi

ZIP_NAME="$(
    find $package_path -maxdepth 1 -type f -name '*.zip' -printf '%T@ %p\n' \
    | sort -nr \
    | head -n 1 \
    | cut -d' ' -f2-
)"

if [ -z "${ZIP_NAME:-}" ]; then
    error_exit "No zip files found in bin/"
fi

echo "🚀 Uploading ${ZIP_NAME} to ${USERNAME}/${GAME}:${CHANNEL} ..."
"${butler}" push "${ZIP_NAME}" "${USERNAME}/${GAME}:${CHANNEL}"
echo "✅ Upload complete."