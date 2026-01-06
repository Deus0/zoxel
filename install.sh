#!/usr/bin/env bash
set -euo pipefail

APP="zoxel"
VERSION="1.0"
BINARY_PATH="./bin/zoxel"
RES_DIR="./res"
ICON_REL="res/textures/game.png"
ICON_PATH="./${ICON_REL}"

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

DESKTOP_CONTENT=$'[Desktop Entry]\nName=Zoxel\nExec=/zoxel\nIcon=zoxel\nType=Application\nCategories=Game;Utility;\nTerminal=false\nStartupWMClass=Zoxel\n'

##############################
# Build Debian package
##############################
build_deb() {
    DEB_DIR="pkg"
    rm -rf "$DEB_DIR"
    mkdir -p "$DEB_DIR/DEBIAN" \
             "$DEB_DIR/bin" \
             "$DEB_DIR/res" \
             "$DEB_DIR/usr/share/applications"

    # Copy binary
    install -Dm755 "${BINARY_PATH}" "$DEB_DIR/${APP}"

    # Copy resources
    cp -a "${RES_DIR}/." "$DEB_DIR/res/"

    # Copy the Iccns
    mkdir -p "$DEB_DIR/usr/share/icons/hicolor/256x256/apps"
    install -Dm644 "${ICON_PATH}" "$DEB_DIR/usr/share/icons/hicolor/256x256/apps/zoxel.png"

    # Desktop entry
    printf '%s\n' "$DESKTOP_CONTENT" > "$DEB_DIR/usr/share/applications/${APP}.desktop"

    # Control file
    cat > "$DEB_DIR/DEBIAN/control" <<EOF
Package: ${APP}
Version: ${VERSION}-1
Section: games
Priority: optional
Architecture: ${ARCH}
Depends: libsdl2-2.0-0 libsdl2-image-2.0-0 libsdl2-mixer-2.0-0
Maintainer: Packager <root>
Description: A voxel game ${APP}
EOF

    # Build and install
    dpkg-deb --build "$DEB_DIR"
    echo "Built ${DEB_DIR}.deb"
    sudo dpkg -i "${DEB_DIR}.deb"
}

##############################
# Build Arch package
##############################
build_arch() {
    ARCH_DIR="pkg"
    rm -rf "$ARCH_DIR"
    mkdir -p "$ARCH_DIR"
    # mkdir -p "$ARCH_DIR/res"

    # Copy sources directly into PKGBUILD folder (makepkg expects them here)
    cp "${BINARY_PATH}" "$ARCH_DIR/zoxel"
    # cp -a "${RES_DIR}/." "$ARCH_DIR/res"
    tar czf  "$ARCH_DIR/res.tar.gz" -C "${RES_DIR}/.." res
    printf '%s\n' "$DESKTOP_CONTENT" > "$ARCH_DIR/zoxel.desktop"

    # PKGBUILD
    cat > "$ARCH_DIR/PKGBUILD" <<'EOF'
pkgname=zoxel
pkgver=1.0
pkgrel=1
pkgdesc="Prebuilt zoxel package with resources and desktop shortcut"
arch=('x86_64')
url="https://codeberg.org/deus/zoxel"
license=('MIT')
source=("zoxel" "zoxel.desktop" "res.tar.gz")
sha256sums=('SKIP' 'SKIP' 'SKIP')
depends=('sdl2' 'sdl2_image' 'sdl2_mixer')

package() {
    install_bin_path="${pkgdir}/usr/bin"
    install -Dm755 "zoxel" "${install_bin_path}/zoxel"

    # Extract the resources archive into the package
    install_res_path="${pkgdir}/usr/share/zoxel"
    mkdir -p "${install_res_path}"
    tar xzf "$srcdir/res.tar.gz" -C "${install_res_path}"

    # install our icon texture
    install -Dm644 "$srcdir/res/textures/game.png" "${pkgdir}/usr/share/icons/hicolor/256x256/apps/zoxel.png" || true

    # install our desktop file
    install -Dm644 "${srcdir}/zoxel.desktop" "${pkgdir}/usr/share/applications/zoxel.desktop"
}
EOF

    # Build and install Arch package
    (cd "$ARCH_DIR" && makepkg --force --noconfirm)
    PKG_FILE=$(find "$ARCH_DIR" -maxdepth 1 -name "*.pkg.tar.*" | head -n1)
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
if [[ "$DISTRO" == "debian" ]]; then
    build_deb
elif [[ "$DISTRO" == "arch" ]]; then
    build_arch
fi
