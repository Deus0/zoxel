#!/usr/bin/env bash
# Looks at the player's character: builds, runs tst body_views with your settings (i.e. Block Res 5),
#   saves tst/body_views.png (4 views round it: +z, +x, -z, above)
#   bash bsh/body.sh              your settings (~/.config/zoxel/settings.cfg)
#   bash bsh/body.sh --nobuild    (and any other bsh/tst.sh arguments, i.e. --1080p for a bigger image)
#   zox_tst_settings= bash bsh/body.sh    the tests' default settings (Block Res 4)
set -uo pipefail
settings="${zox_tst_settings-$HOME/.config/zoxel/settings.cfg}"
[[ -n "${settings}" && ! -f "${settings}" ]] && settings=""
output=$(zox_tst_settings="${settings}" bash bsh/tst.sh body_views "$@" 2>&1)
code=$?
echo "${output}" | grep -a "body:\|PASSED\|FAIL\|exit \[\|build failed"
if [[ -f tst/body_views.ppm ]]; then
    if command -v convert > /dev/null; then
        convert tst/body_views.ppm tst/body_views.png && echo "> tst/body_views.png"
    else
        echo "> tst/body_views.ppm"
    fi
fi
exit ${code}
