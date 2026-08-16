#!/usr/bin/env bash
set -euo pipefail

# NOTE: THe desktop file needs same name as binary
date_str=$(date +%Y_%m_%d)
major_version="0"
minor_version="0"
patch_version="1"
VERSION=${date_str} # "${major_version}.${minor_version}.${patch_version}"

game="${1:-zoxel}"
game_name="${game^}"
sdl_version="${2:-SDL3}" # SDL3 option too
OS="linux"
ARC="x64"
GLB="opengl"
GFX="sdl"
package_name="${game}_${OS}_${ARC}_${GLB}_${GFX}_${date_str}"
CHANNEL="${OS}_${ARC}"
lib_path="lib/${CHANNEL}"
libsdl="lib${sdl_version}.so.0"
libsdl_mixer="lib${sdl_version}_mixer.so.0"
sdl_path="${lib_path}/${libsdl}"
sdl_mixer_path="${lib_path}/${libsdl_mixer}"
BINARY_PATH="./bin/${game}.bin"
RES_DIR="./res"
install_path="/usr/local/games"
install_game_path="${install_path}/${game}"
install_res_path="${install_game_path}/res"
ICON_REL="gam/${game}/res/game.png"
ICON_PATH="./${ICON_REL}"
# bin_install_dir="/usr/bin"
APP_ID="org.zox.${game}"
shortcut_install_dir="/usr/share/applications"
desktop_file="${game}.bin.desktop"
shortcut_path="pkg/${desktop_file}"
url_path="https://codeberg.org/deus/zoxel"
staging_dir="pkg"

echo "Installing [${game}] to [${install_game_path}]"
echo "  - lib_path is [${lib_path}]"
echo "  - sdl [${sdl_version}]"
echo "  - game name [${game_name}]"

# Validate Lib path
if [[ ! -d "${lib_path}" ]]; then
    echo "Error: '${lib_path}' directory not found."
    exit 1
fi

# Validate inputs
if [[ ! -x "${BINARY_PATH}" ]]; then
    echo "Error: '${BINARY_PATH}' binary not found or not executable."
    exit 1
fi
if [[ ! -f "${ICON_PATH}" ]]; then
    echo "Error: '${ICON_PATH}' not found in project."
    exit 1
fi
if [[ ! -d "${RES_DIR}" ]]; then
    echo "Error: '${RES_DIR}' directory not found in project."
    exit 1
fi

ARCH=$(uname -m)
case "$ARCH" in
  aarch64) ARCH=arm64 ;;
  x86_64)  ARCH=amd64 ;;
esac

# Detect distro
if command -v apt >/dev/null 2>&1 && command -v dpkg >/dev/null 2>&1; then
    DISTRO="debian"
elif command -v pacman >/dev/null 2>&1 && command -v makepkg >/dev/null 2>&1; then
    DISTRO="arch"
else
    echo "Unsupported distro. Only Debian/Ubuntu or Arch/Manjaro supported."
    exit 1
fi

DESKTOP_CONTENT=$(cat <<EOF
[Desktop Entry]
Name=${game_name}
Icon=${game}
Comment=Voxel Game
Exec=/usr/local/games/${game}/${game}.bin
Terminal=false
Type=Application
Categories=Game;
EOF
)

##############################
# Build Debian package
##############################
build_deb() {
    mkdir -p "${staging_dir}/DEBIAN" \
             "${staging_dir}${shortcut_install_dir}" \
             "${staging_dir}/usr/share/icons/hicolor/256x256/apps"
             #"${staging_dir}${bin_install_dir}" \
             #"${staging_dir}${res_install_dir}" \
             #"${staging_dir}${res_install_dir}/res" \

    mkdir -p "${staging_dir}${install_res_path}"

    # Copy binary
    install -Dm755 "${BINARY_PATH}" "${staging_dir}${install_game_path}/${game}.bin"
    # Copy resources
    cp -a "${RES_DIR}/." "${staging_dir}${install_res_path}"
    # Copy the Iccns
    install -Dm644 "${ICON_PATH}" \
    "${staging_dir}/usr/share/icons/hicolor/256x256/apps/${game}.png"
    # Desktop entry
    install -Dm644 "${shortcut_path}" "${staging_dir}${shortcut_install_dir}/${desktop_file}"

    # Control file
    cat > "$staging_dir/DEBIAN/control" <<EOF
Package: ${game}
Version: ${VERSION}
Section: games
Priority: optional
Architecture: ${ARCH}
Depends: libsdl2-2.0-0, libsdl2-mixer-2.0-0
Maintainer: Packager <root>
Description: A Voxel Game ${game}
EOF
    # Build and install
    # dpkg-deb --build "$staging_dir"
    dpkg-deb --root-owner-group --build "$staging_dir"
    echo "Built ${staging_dir}.deb"
    sudo dpkg -i "${staging_dir}.deb"
}

##############################
# Build Arch package
##############################
build_arch() {
    mkdir -p "${staging_dir}"
    # Copy sources directly into PKGBUILD folder (makepkg expects them here)
    cp "${BINARY_PATH}" "${staging_dir}/${game}"
    cp "${sdl_path}" "${staging_dir}"
    cp "${sdl_mixer_path}" "${staging_dir}"
    tar czf  "${staging_dir}/res.tar.gz" -C "${RES_DIR}/.." res
    cp "${ICON_PATH}" "${staging_dir}/game.png"
    # PKGBUILD
    cat > "${staging_dir}/PKGBUILD" <<EOF
pkgname="${game}"
pkgver="${VERSION}"
pkgrel=0
pkgdesc="An awesome game [${game}]"
arch=('x86_64')
url="${url_path}"
license=('GPL3')
source=("${game}" "${libsdl}" "${libsdl_mixer}" "${desktop_file}" "res.tar.gz")
sha256sums=('SKIP' 'SKIP' 'SKIP' 'SKIP' 'SKIP')
depends=()

package() {
    install_game_path="\${pkgdir}/usr/local/games/\${pkgname}"
    install_res_path="\${install_game_path}/res"
    mkdir -p "\${install_res_path}"
    install -Dm755 "\${pkgname}" "\${install_game_path}/\${pkgname}.bin"
    install -Dm644 "${libsdl}" "\${install_game_path}/${libsdl}"
    install -Dm644 "${libsdl_mixer}" "\${install_game_path}/${libsdl_mixer}"
    tar xzf "\${srcdir}/res.tar.gz" \
        -C "\${install_game_path}"
    install -Dm644 "\${srcdir}/../game.png" \
        "\${pkgdir}/usr/share/icons/hicolor/256x256/apps/\${pkgname}.png"
        install -Dm644 "\${srcdir}/${desktop_file}" \
    "\${pkgdir}/usr/share/applications/${desktop_file}"
}
EOF
    # Build and install Arch package
    (cd "$staging_dir" && makepkg --force --noconfirm)
    PKG_FILE=$(find "$staging_dir" -maxdepth 1 -name "*.pkg.tar.*" | head -n1)
    if [[ -z "$PKG_FILE" || ! -f "$PKG_FILE" ]]; then
        echo "Error: no Arch package found to install."
        exit 1
    fi
    PACKAGE_FILE="zip/${package_name}.pkg.tar.zst"
    mv "${PKG_FILE}" "${PACKAGE_FILE}"
    sudo pacman -U --noconfirm "${PACKAGE_FILE}"
    echo "Arch package installed: ${PACKAGE_FILE}"
}

##############################
# Main
##############################

# make our desktop file
rm -rf pkg
mkdir -p pkg
printf '%s\n' "${DESKTOP_CONTENT}" > "${shortcut_path}"

if [[ "$DISTRO" == "debian" ]]; then
    build_deb
elif [[ "$DISTRO" == "arch" ]]; then
    build_arch
fi
