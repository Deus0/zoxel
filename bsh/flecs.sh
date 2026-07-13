#!/usr/bin/env bash
set -euo pipefail
# Shell to download Flecs

# Requirements: curl
version=4.1.6
url=https://raw.githubusercontent.com/SanderMertens/flecs/v${version}/distr

if test -f inc/flecs/flecs.c && test -f inc/flecs/flecs.h; then
    echo "> Flecs Source Found"
    echo "  - [inc/flecs/flecs.c]"
    echo "  - [inc/flecs/flecs.h]"
    sleep 1
    exit 0
fi

echo "-> Downloading Flecs v${version}"
echo "  @ [${url}]"

if command -v curl >/dev/null 2>&1; then
    echo "-> [curl] is available"
    sleep 1
else
    echo "!! [curl] not found"
    sleep 2
    exit 1
fi

# make sure folders exist
echo "-> Created [inc/flecs]"
echo ""
mkdir -p inc inc/flecs
sleep 1

# wget ${url}/flecs.h -O flecs.h && wget ${url}/flecs.c -O flecs.c || { echo >&2 "Download failed"; exit 1; }

echo "-> Downloading [inc/flecs/flecs.h]..."
curl -fs "${url}/flecs.h" -o inc/flecs/flecs.h || \
    { echo >&2 "Download failed"; exit 1; }
echo "+ Download Complete"
sleep 1

echo "-> Downloading [inc/flecs/flecs.c]..."
curl -fs "${url}/flecs.c" -o inc/flecs/flecs.c || \
    { echo >&2 "Download failed"; exit 1; }
echo "+ Download Complete"
sleep 1

echo ""
echo "+ Flecs Fetching Complete!"
echo ""

#if pkg-config --exists flecs; then \
#    echo "> Flecs Package Installed"; \
#else \
