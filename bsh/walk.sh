#!/usr/bin/env bash
# Looks at the walk from 4 angles (tst walk_views): builds, runs it with your settings (i.e. Block Res 5),
#   saves tst/walk_views/walk.gif and tst/walk_views.png (its frames in a grid)
#   bash bsh/walk.sh              your settings (~/.config/zoxel/settings.cfg)
#   bash bsh/walk.sh --nobuild    (and any other bsh/tst.sh arguments, i.e. --bones)
#   zox_tst_gif_fps=30 bash bsh/walk.sh    frames a second (20 by default)
set -uo pipefail
settings="${zox_tst_settings-$HOME/.config/zoxel/settings.cfg}"
[[ -n "${settings}" && ! -f "${settings}" ]] && settings=""
dir="${zox_tst_dir:-tst}"
rm -f "${dir}"/walk_views/*.gif
# the game's size by default (the tests' window is small for looking at)
[[ " $* " == *" --1080p "* ]] || set -- "$@" --1080p
output=$(zox_tst_settings="${settings}" bash bsh/tst.sh walk_views "$@" 2>&1)
code=$?
echo "${output}" | grep -a "\] walk_views: \|PASSED\|FAIL\|exit \[\|build failed" | grep -av "^ *|"
gif="${dir}/walk_views/walk.gif"
if [[ -f "${gif}" ]] && command -v magick > /dev/null; then
    # every other frame, half size
    frames=$(mktemp -d)
    magick "${gif}" -coalesce -resize 50% "${frames}/%03d.png"
    shopt -s nullglob
    files=("${frames}"/*[02468].png)
    magick montage "${files[@]}" -tile 6x -geometry +2+2 -background "#303030" "${dir}/walk_views.png" \
        && echo "> ${dir}/walk_views.png"
    rm -rf "${frames}"
fi
exit ${code}
