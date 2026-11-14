#!/usr/bin/env bash
# Looks at gpu meshing's lighting: a torch lit cave drawn 4 ways (tst gpu_light_views, doc/tec/gpu_meshing.md),
#   saves tst/gpu_light_views.png: unlit (top left), face (top right), vertex (bottom left, as cpu meshes are lit),
#   pixel (bottom right, bilinear across each face)
#   bash bsh/gpu_lights.sh              software rendering (as tests)
#   bash bsh/gpu_lights.sh --1080p      bigger, to see the difference between vertex and pixel
#   bash bsh/gpu_lights.sh --visible    on the real gpu
set -uo pipefail
output=$(bash bsh/tst.sh gpu_light_views "$@" 2>&1)
code=$?
echo "${output}" | grep -a "gpu_light_views\|copy \[\|PASSED\|FAIL\|exit \[\|build failed" | sed 's/^.*\[tst\] //'
if [[ -f tst/gpu_light_views.ppm ]]; then
    if command -v convert > /dev/null; then
        convert tst/gpu_light_views.ppm tst/gpu_light_views.png && echo "> tst/gpu_light_views.png"
    else
        echo "> tst/gpu_light_views.ppm"
    fi
fi
exit ${code}
