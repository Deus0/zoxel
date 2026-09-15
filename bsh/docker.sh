#!/bin/bash

set -e

game_name="zoxel"
docker_image="ubuntu:24.04"
CONTAINER_NAME="zox24"
ARGS=("$@")

# if not a -- we set our game name
if [[ $# -gt 0 && ${1} != --* ]]; then
    game_name="$1"
fi
# ${game_name}
# change later to zox
mount_point="/zoxel"

# x64 Docker build on ARM
DOCKER_PLATFORM=()
DOCKER_PLATFORM_NAME="native"
if [[ " ${ARGS[*]} " == *" --x64 "* ]] && [[ "$(uname -m)" == "aarch64" || "$(uname -m)" == "arm64" ]]; then
    DOCKER_PLATFORM=(--platform linux/amd64)
    DOCKER_PLATFORM_NAME="x64"
fi

if [[ " $* " == *" --ubuntu20 "* ]]; then
    docker_image="ubuntu:20.04"
    CONTAINER_NAME="zox20"
    echo "Enabling [${docker_image}]"
fi

if [[ " $* " == *" --ubuntu22 "* ]]; then
    docker_image="ubuntu:22.04"
    CONTAINER_NAME="zox22"
    echo "Enabling [${docker_image}]"
fi

if [[ " $* " == *" --ubuntu24 "* ]]; then
    docker_image="ubuntu:24.04"
    CONTAINER_NAME="zox24"
    echo "Enabling [${docker_image}]"
fi

CONTAINER_NAME="${CONTAINER_NAME}_${DOCKER_PLATFORM_NAME}"
echo "Docker Container Name [${CONTAINER_NAME}]"

# Uncomment to reset
if [[ " $* " == *" --reset "* ]]; then
    echo "Removing Container [${CONTAINER_NAME}]"
    docker rm -f "${CONTAINER_NAME}" 2>/dev/null || true
fi

if [[ " $* " == *" --remove "* ]]; then
    echo "Removing Container [${CONTAINER_NAME}]"
    docker rm -f "${CONTAINER_NAME}" 2>/dev/null || true
    exit
fi

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

# Ensure user belongs to docker group.
if ! getent group docker | grep -qw "$USER"; then
    echo "👤 Adding $USER to docker group..."
    sudo usermod -aG docker "$USER"
fi


# Start Docker if necessary.
if ! systemctl is-active --quiet docker; then
    echo "🐳 Starting Docker..."
    sudo systemctl enable --now docker
fi

# Check whether this shell can access Docker.
if ! docker info >/dev/null 2>&1; then
    echo "🐳 Refreshing Docker group access..."

    # If docker group membership was just added, execute the
    # remainder of this script inside a docker-group shell.
    exec newgrp docker <<< "$0 ${ARGS[*]}"
fi

echo "🐳 Docker ready."

if ! docker container inspect "$CONTAINER_NAME" >/dev/null 2>&1; then
    echo "📦 Creating ${docker_image} build container..."

    docker run -dit \
        "${DOCKER_PLATFORM[@]}" \
        --name "$CONTAINER_NAME" \
        -v "$PROJECT_DIR:${mount_point}" \
        -w "${mount_point}" \
        ${docker_image} \
        bash
else
    echo "📦 Reusing ${docker_image} build container."
fi

if ! docker container inspect -f '{{.State.Running}}' "$CONTAINER_NAME" 2>/dev/null | grep -q true; then
    echo "📦 Starting build container..."
    docker start "$CONTAINER_NAME" >/dev/null
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
echo "🔨 Building ${game_name}..."
echo "  - Args [${ARGS[@]}] -"
echo

docker exec \
    -u "$(id -u):$(id -g)" \
    "$CONTAINER_NAME" \
    bash -c "cd '${mount_point}' && bash bsh/build.sh \"\$@\" --package --docker" \
    bash "${ARGS[@]}"

    #bash -c 'cd ${mount_point} && bash bsh/build.sh "$@" --package --docker' \
    #bash "${ARGS[@]}"

echo
echo "✅ Build [${game_name}] complete."
