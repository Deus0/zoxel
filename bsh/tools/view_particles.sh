#!/usr/bin/env bash
# Record the player's particle bursts to tst/particles/particles.gif.
# Captures use tiny, flat terrain and hide UI and NPCs.
# Pass --readme to publish the resized GIF to doc/gifs/particles.gif.
set -euo pipefail
cd "$(dirname "$0")/../.."
export zox_tst_dir="${zox_tst_dir:-tst/particles}"
bash bsh/tst.sh aesthetic_particles --nouis --tiny --nonpcs --flatlands "$@"
if [[ ! -s "${zox_tst_dir}/particles.gif" ]]; then
    echo "!! missing GIF [${zox_tst_dir}/particles.gif]" >&2
    exit 1
fi
echo "> GIF [${zox_tst_dir}/particles.gif]"
