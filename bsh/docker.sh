#!/bin/bash

set -e

CONTAINER_NAME="zoxel-linux20"

# Uncomment to reset
# docker rm -f ${CONTAINER_NAME}

# Uncomment to remove libs
# rm -f "$PROJECT_DIR"/lib/linux_x64/*.so

PROJECT_DIR="$(cd "$(dirname "$0")/.." && pwd)"

# Build tools
build_packages=(
    build-essential
    cmake
    zip
    pkg-config
)

# OpenGL
opengl_packages=(
    libgl1-mesa-dev
)

# X11 desktop windowing
# X11 desktop windowing
x11_packages=(
    libx11-dev
    libxext-dev
    libxrandr-dev
    libxinerama-dev
    libxcursor-dev
    libxi-dev
    libxfixes-dev
    libxss-dev
    libxtst-dev
)

# Wayland desktop windowing
wayland_packages=(
    libwayland-dev
    libxkbcommon-dev
)

# ALSA audio
audio_packages=(
    libasound2-dev
)

packages=(
    "${build_packages[@]}"
    "${opengl_packages[@]}"
    "${x11_packages[@]}"
    "${wayland_packages[@]}"
    "${audio_packages[@]}"
)

# git
# libglu1-mesa-dev

echo "🐳 Checking Docker..."

if ! command -v docker >/dev/null 2>&1; then
    echo "🐳 Docker not found. Installing..."

    if command -v pacman >/dev/null 2>&1; then
        echo "Installing Docker with pacman..."
        sudo pacman -S --needed docker
    elif command -v apt >/dev/null 2>&1; then
        echo "Installing Docker with apt..."
        sudo apt update
        sudo apt install -y docker.io
    else
        echo "ERROR: Could not determine package manager."
        echo "Please install Docker manually."
        exit 1
    fi
    echo "Need to restart shell, due to docker install"
    newgrp docker
    exit 1
fi

if ! getent group docker >/dev/null 2>&1; then
    echo "🐳 Creating docker group..."
    sudo groupadd docker
fi

if ! id -nG "$USER" | grep -qw docker; then
    echo "👤 Adding $USER to docker group..."
    sudo usermod -aG docker "$USER"

    echo
    echo "Docker access has been added."
    echo "Run:"
    echo
    echo "    newgrp docker"
    echo
    echo "Then run this script again."
    exit 0
fi

if ! systemctl is-active --quiet docker; then
    echo "Starting Docker..."
    sudo systemctl enable --now docker
fi

if ! docker info >/dev/null 2>&1; then
    echo
    echo "ERROR: Docker is running but this shell does not have Docker access."
    echo "Run:"
    echo
    echo "    newgrp docker"
    echo
    exit 1
fi

echo "🐳 Docker ready."

if ! docker container inspect "$CONTAINER_NAME" >/dev/null 2>&1; then
    echo "📦 Creating Ubuntu 20.04 build container..."

    docker run -dit \
        --name "$CONTAINER_NAME" \
        -v "$PROJECT_DIR:/zoxel" \
        -w /zoxel \
        ubuntu:20.04 \
        bash
else
    echo "📦 Reusing Ubuntu 20.04 build container."
fi

echo "📦 Updating System [$CONTAINER_NAME]"
docker exec \
    -e DEBIAN_FRONTEND=noninteractive \
    "$CONTAINER_NAME" \
    apt update

echo "📦 Installing build dependencies..."
printf '   %s\n' "${packages[@]}"
docker exec \
    -e DEBIAN_FRONTEND=noninteractive \
    "$CONTAINER_NAME" \
    apt install -y "${packages[@]}"

echo
echo "🔨 Building Zoxel..."
echo

docker exec \
    "$CONTAINER_NAME" \
    bash -c 'cd /zoxel && bash bsh/linux.sh --package'

echo
echo "✅ Zoxel build complete."