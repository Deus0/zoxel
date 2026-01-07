#!/usr/bin/env bash
set -euo pipefail

# get latest, build, then install new if updated
git pull && make && ./install.sh
echo "[zoxel] Update has completed"