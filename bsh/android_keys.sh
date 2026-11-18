#!/usr/bin/env bash
set -euo pipefail
# android signing key, made once in ~/.config/zox_keys, reused by every signed build (bsh/android.sh)
#   bash bsh/android_keys.sh [game]            makes the keystore and password if they don't exist
#   bash bsh/android_keys.sh [game] --github   and saves them as github secrets, so ci signs with the same key
#                                              (needs gh: sudo pacman -S github-cli, then gh auth login)
# back up both files: a lost key means installed apks can't be updated, only reinstalled

game_name="zoxel"
[[ $# -gt 0 && ${1} != --* ]] && game_name="$1"
keystore_dir="${HOME}/.config/zox_keys"
keystore_path="${keystore_dir}/${game_name}.keystore"
password_path="${keystore_dir}/${game_name}.password"

mkdir -p "${keystore_dir}"
chmod 700 "${keystore_dir}"

if [[ ! -f "${password_path}" ]]; then
    head -c 32 /dev/urandom | base64 > "${password_path}"
    chmod 600 "${password_path}"
    echo "+ Created keystore password [${password_path}]"
fi
password="$(head -n 1 "${password_path}")"
[[ -z "${password}" ]] && { echo "ERROR: empty password file [${password_path}]"; exit 1; }

if [[ ! -f "${keystore_path}" ]]; then
    keytool -genkeypair -keystore "${keystore_path}" -storepass "${password}" -keypass "${password}" \
        -alias "${game_name}" -keyalg RSA -keysize 2048 -validity 10000 -dname "CN=${game_name^}"
    chmod 600 "${keystore_path}"
    echo "+ Created keystore [${keystore_path}], back it up with its password"
else
    echo "- Keystore found [${keystore_path}]"
fi

if [[ " $* " == *" --github "* ]]; then
    command -v gh >/dev/null 2>&1 || { echo "ERROR: needs gh: sudo pacman -S github-cli, then gh auth login"; exit 1; }
    repo="deus0/zoxel"
    base64 -w0 "${keystore_path}" | gh secret set ANDROID_KEYSTORE --repo "${repo}"
    printf '%s' "${password}" | gh secret set ANDROID_KEYSTORE_PASSWORD --repo "${repo}"
    echo "+ Saved ANDROID_KEYSTORE and ANDROID_KEYSTORE_PASSWORD to github [${repo}]"
fi
