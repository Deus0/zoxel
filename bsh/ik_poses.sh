#!/usr/bin/env bash
# Looks at the player's character posed by its ik rig (reaching, wrists, leaning, crouching: tst ik_poses):
#   builds, runs tst ik_poses with your settings (i.e. Block Res 5), saves tst/ik_poses.png
#   (a row a pose: +z, +x, -z, above views)
#   bash bsh/ik_poses.sh              your settings (~/.config/zoxel/settings.cfg)
#   bash bsh/ik_poses.sh --nobuild    (and any other bsh/tst.sh arguments, i.e. --1080p for a bigger image)
#   zox_tst_settings= bash bsh/ik_poses.sh    the tests' default settings (Block Res 4)
set -uo pipefail
settings="${zox_tst_settings-$HOME/.config/zoxel/settings.cfg}"
[[ -n "${settings}" && ! -f "${settings}" ]] && settings=""
rm -f tst/ik_poses/*.ppm
# the game's size by default (the tests' window is small for looking at)
[[ " $* " == *" --1080p "* ]] || set -- "$@" --1080p
output=$(zox_tst_settings="${settings}" bash bsh/tst.sh ik_poses "$@" 2>&1)
code=$?
echo "${output}" | grep -a "\] ik pose \[\|metric\|PASSED\|FAIL\|exit \[\|build failed" | grep -av "^ *|"
shopt -s nullglob
poses=(tst/ik_poses/*.ppm)
if [[ ${#poses[@]} -gt 0 ]] && command -v montage > /dev/null; then
    args=()
    for pose in "${poses[@]}"; do
        name=$(basename "${pose}" .ppm)
        args+=(-label "${name}" "${pose}")
    done
    montage "${args[@]}" -tile 4x -geometry +4+4 -background "#303030" -fill white tst/ik_poses.png \
        && echo "> tst/ik_poses.png"
fi
exit ${code}
