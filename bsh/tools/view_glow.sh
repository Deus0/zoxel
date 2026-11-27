#!/usr/bin/env bash
# Glowing things in the dark through the game's camera: clamp, bloom, soft and filmic tonemaps with bloom
#   (doc/plans/hdr.md). Default: a glow stone in a brick room (tst glow_views); --lamp: a street lamp in a stone cave
#   (tst lamp_views). Saves tst/glow/<test>/*.png. --vulkan for the vulkan backend, --nobuild to rerun.
set -euo pipefail
cd "$(dirname "$0")/../.."
export zox_tst_dir="${zox_tst_dir:-tst/glow}"
test=glow_views
args=()
for arg in "$@"; do
    if [[ "${arg}" == "--lamp" ]]; then
        test=lamp_views
    else
        args+=("${arg}")
    fi
done
bash bsh/tst.sh "${test}" "${args[@]}"
if command -v magick > /dev/null; then
    for ppm in "${zox_tst_dir}/${test}"/*.ppm; do
        [[ -f "${ppm}" ]] && magick "${ppm}" "${ppm%.ppm}.png"
    done
fi
ls "${zox_tst_dir}/${test}" 2> /dev/null | grep png || true
