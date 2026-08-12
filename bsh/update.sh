#!/usr/bin/env bash
set -euo pipefail

# get latest, build, then install new if updated
git pull && ./build.sh && ./install.sh
echo "[zoxel] Update has completed"