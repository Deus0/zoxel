#!/usr/bin/env bash
# A glow stone in a dark brick room through the game's camera: clamp, bloom, soft and filmic tonemaps with bloom
#   (tst glow_views, doc/plans/hdr.md). Saves tst/glow/glow_*.png. --vulkan for the vulkan backend, --nobuild to rerun.
set -euo pipefail
cd "$(dirname "$0")/../.."
export zox_tst_dir="${zox_tst_dir:-tst/glow}"
bash bsh/tst.sh glow_views "$@"
if command -v magick > /dev/null; then
    for ppm in "${zox_tst_dir}"/glow_views/glow_*.ppm "${zox_tst_dir}"/glow_*.ppm; do
        [[ -f "${ppm}" ]] && magick "${ppm}" "${ppm%.ppm}.png"
    done
fi
ls "${zox_tst_dir}" "${zox_tst_dir}/glow_views" 2> /dev/null | grep png || true
