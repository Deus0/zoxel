#!/usr/bin/env bash
# Looks at the player's character in poses (arms forward, up, out, a stride, sitting...) for holes in its skin:
#   builds, runs tst body_poses with your settings (i.e. Block Res 5), saves tst/body_poses.png
#   (a row a pose: +z, +x, -z, above views; holes, the background showing inside the body, painted magenta)
#   bash bsh/poses.sh              your settings (~/.config/zoxel/settings.cfg)
#   bash bsh/poses.sh --nobuild    (and any other bsh/tst.sh arguments, i.e. --1080p for a bigger image)
#   zox_tst_settings= bash bsh/poses.sh    the tests' default settings (Block Res 4)
#   bash bsh/poses.sh --weights    the skin painted by bone instead (tst body_weights), saves tst/body_weights.png
set -uo pipefail
name=body_poses
args=()
for arg in "$@"; do
    if [[ "${arg}" == "--weights" ]]; then
        name=body_weights
    else
        args+=("${arg}")
    fi
done
set -- ${args[@]+"${args[@]}"}
settings="${zox_tst_settings-$HOME/.config/zoxel/settings.cfg}"
[[ -n "${settings}" && ! -f "${settings}" ]] && settings=""
rm -f tst/${name}/*.ppm
# the game's size by default (the tests' window is small for looking at)
[[ " $* " == *" --1080p "* ]] || set -- "$@" --1080p
output=$(zox_tst_settings="${settings}" bash bsh/tst.sh ${name} "$@" 2>&1)
code=$?
echo "${output}" | grep -a "\] pose \[\|\] corners between\|metric\|PASSED\|FAIL\|exit \[\|build failed" | grep -av "^ *|"
shopt -s nullglob
poses=(tst/${name}/*.ppm)
if [[ ${#poses[@]} -gt 0 ]] && command -v montage > /dev/null; then
    labels=()
    for pose in "${poses[@]}"; do
        labels+=(-label "$(basename "${pose}" .ppm)" "${pose}")
    done
    montage "${labels[@]}" -tile 4x -geometry +4+4 -background "#303030" -fill white tst/${name}.png \
        && echo "> tst/${name}.png"
fi
exit ${code}
