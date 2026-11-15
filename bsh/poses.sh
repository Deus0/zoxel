#!/usr/bin/env bash
# Looks at the player's character in poses (arms forward, up, out, a stride, sitting...) for holes in its skin:
#   builds, runs tst body_poses with your settings (i.e. Block Res 5), saves tst/body_poses.png
#   (a row a pose: +z, +x, -z, above views; holes, the background showing inside the body, painted magenta)
#   bash bsh/poses.sh              your settings (~/.config/zoxel/settings.cfg)
#   bash bsh/poses.sh --nobuild    (and any other bsh/tst.sh arguments, i.e. --1080p for a bigger image)
#   zox_tst_settings= bash bsh/poses.sh    the tests' default settings (Block Res 4)
set -uo pipefail
settings="${zox_tst_settings-$HOME/.config/zoxel/settings.cfg}"
[[ -n "${settings}" && ! -f "${settings}" ]] && settings=""
rm -f tst/body_poses/*.ppm
# the game's size by default (the tests' window is small for looking at)
[[ " $* " == *" --1080p "* ]] || set -- "$@" --1080p
output=$(zox_tst_settings="${settings}" bash bsh/tst.sh body_poses "$@" 2>&1)
code=$?
echo "${output}" | grep -a "\] pose \[\|\] corners between\|metric\|PASSED\|FAIL\|exit \[\|build failed" | grep -av "^ *|"
shopt -s nullglob
poses=(tst/body_poses/*.ppm)
if [[ ${#poses[@]} -gt 0 ]] && command -v montage > /dev/null; then
    args=()
    for pose in "${poses[@]}"; do
        name=$(basename "${pose}" .ppm)
        args+=(-label "${name}" "${pose}")
    done
    montage "${args[@]}" -tile 4x -geometry +4+4 -background "#303030" -fill white tst/body_poses.png \
        && echo "> tst/body_poses.png"
fi
exit ${code}
