#!/usr/bin/env bash
# Looks at a realm model: builds, runs tst model_views, saves tst/models/<name>.png (4 views on gray)
#   bash bsh/model.sh              the realm's first model
#   bash bsh/model.sh Shirt        the model whose name holds Shirt (the realm's models are listed)
#   bash bsh/model.sh Shirt --nobuild   (and any other bsh/tst.sh arguments, i.e. --1080p for a bigger image)
# For iterating a blueprint: edit it, run this, look at the png
set -uo pipefail
model="${1:-}"
[[ $# -gt 0 ]] && shift
output=$(zox_tst_model="${model}" bash bsh/tst.sh model_views "$@" 2>&1)
code=$?
echo "${output}" | grep -a "realm models\|looking at\|saved\|PASSED\|FAIL\|exit \[\|build failed"
saved=$(echo "${output}" | grep -aoP "\[tst\] saved \[\K[^\]]+" | tail -1)
if [[ -n "${saved}" && -f "${saved}" ]]; then
    png="${saved%.ppm}.png"
    if command -v convert > /dev/null; then
        convert "${saved}" "${png}" && echo "> ${png}"
    else
        echo "> ${saved} (no imagemagick convert for a png)"
    fi
fi
exit ${code}
