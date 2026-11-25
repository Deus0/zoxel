#!/usr/bin/env bash
# Capture the town wall, town interior, and landscape to tst/town/aesthetic/.
# Pass --readme to publish the resized screenshots to doc/screenshot/.
set -euo pipefail
cd "$(dirname "$0")/../.."
export zox_tst_dir="${zox_tst_dir:-tst/town}"
bash bsh/tst.sh aesthetic_town "$@"
echo "> Screenshots [${zox_tst_dir}/aesthetic/]"
