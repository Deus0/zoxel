#!/usr/bin/env bash
set -euo pipefail

APP="zoxel"
VERSION="1.0"
BINARY_PATH="./bin/${APP}"
RES_DIR="./res"
ICON_REL="res/textures/game.png"
ICON_PATH="./${ICON_REL}"
bin_install_dir="/usr/bin"
res_install_dir="/usr/share/${APP}"
shortcut_install_dir="/usr/share/applications"
shortcut_path="pkg/${APP}.desktop"
url_path="https://codeberg.org/deus/zoxel"
staging_dir="pkg"

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
Name=$APP
Exec=$APP
Icon=$APP
Type=Application
Categories=Game;Utility;
Terminal=false
StartupWMClass=$APP
EOF
)

##############################
# Build Debian package
##############################
build_deb() {
    mkdir -p "${staging_dir}/DEBIAN" \
             "${staging_dir}${bin_install_dir}" \
             "${staging_dir}${res_install_dir}" \
             "${staging_dir}${res_install_dir}/res" \
             "${staging_dir}${shortcut_install_dir}"

    # Copy binary
    install -Dm755 "${BINARY_PATH}" "${staging_dir}${bin_install_dir}/${APP}"

    # Copy resources
    cp -a "${RES_DIR}/." "${staging_dir}${res_install_dir}/res/"

    # Copy the Iccns
    mkdir -p "${staging_dir}/usr/share/icons/hicolor/256x256/apps"
    install -Dm644 "${ICON_PATH}" "${staging_dir}/usr/share/icons/hicolor/256x256/apps/${APP}.png"

    # Desktop entry
    install -Dm644 "${shortcut_path}" "${staging_dir}${shortcut_install_dir}${APP}.desktop"

    # Control file
    cat > "$staging_dir/DEBIAN/control" <<EOF
Package: ${APP}
Version: ${VERSION}-1
Section: games
Priority: optional
Architecture: ${ARCH}
Depends: libsdl2-2.0-0, libsdl2-image-2.0-0, libsdl2-mixer-2.0-0
Maintainer: Packager <root>
Description: A voxel game ${APP}
EOF

    # Build and install
    dpkg-deb --build "$staging_dir"
    echo "Built ${staging_dir}.deb"
    sudo dpkg -i "${staging_dir}.deb"
}

##############################
# Build Arch package
##############################
build_arch() {
    mkdir -p "$staging_dir"
    # Copy sources directly into PKGBUILD folder (makepkg expects them here)
    cp "${BINARY_PATH}" "$staging_dir/${APP}"
    # cp -a "${RES_DIR}/." "$staging_dir/res"
    tar czf  "$staging_dir/res.tar.gz" -C "${RES_DIR}/.." res
    # PKGBUILD
    cat > "$staging_dir/PKGBUILD" <<EOF
pkgname="${APP}"
pkgver=1.0
pkgrel=1
pkgdesc="An awesome game [${APP}]"
arch=('x86_64')
url="${url_path}"
license=('GPL3')
source=("${APP}" "${APP}.desktop" "res.tar.gz")
sha256sums=('SKIP' 'SKIP' 'SKIP')
depends=('sdl2' 'sdl2_image' 'sdl2_mixer')

package() {
    install_bin_path="\${pkgdir}/usr/bin"
    install_res_path="\${pkgdir}/usr/share/\${pkgname}"
    mkdir -p "\${install_res_path}"
    install -Dm755 "\${pkgname}" "\${install_bin_path}/\${pkgname}"
    tar xzf "\$srcdir/res.tar.gz" -C "\${install_res_path}"
    install -Dm644 "\$srcdir/res/textures/game.png" "\${pkgdir}/usr/share/icons/hicolor/256x256/apps/\${pkgname}.png"
    install -Dm644 "\$srcdir/\${pkgname}.desktop" "\${pkgdir}/usr/share/applications/\${pkgname}.desktop"
}
EOF

    # Build and install Arch package
    (cd "$staging_dir" && makepkg --force --noconfirm)
    PKG_FILE=$(find "$staging_dir" -maxdepth 1 -name "*.pkg.tar.*" | head -n1)
    if [[ -z "$PKG_FILE" || ! -f "$PKG_FILE" ]]; then
        echo "Error: no Arch package found to install."
        exit 1
    fi
    sudo pacman -U --noconfirm "$PKG_FILE"
    echo "Arch package installed."
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
