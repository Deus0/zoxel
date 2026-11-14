#!/usr/bin/env bash
# Stress tests gpu meshing with lights: terrain chunks at night re-lit by moving torches (and re-shaped) every frame,
#   meshed lit on the gpu, drawn with pixel light (tst gpu_light_stress, doc/tec/gpu_meshing.md)
#   logs chunks updated a second and each part's time, saves tst/gpu_light_stress.png
#   bash bsh/gpu_light_stress.sh                      4 chunks, 6 seconds, software rendering (as tests)
#   bash bsh/gpu_light_stress.sh --visible            watch it on the real gpu (20 seconds)
#   zox_tst_gpu_chunks=16 bash bsh/gpu_light_stress.sh   more chunks (up to 16, 4 in view)
#   zox_tst_gpu_voxels=0 bash bsh/gpu_light_stress.sh    lights only, the hills stay still
#   zox_tst_gpu_seconds=60 bash bsh/gpu_light_stress.sh --visible --nobuild   longer
set -uo pipefail
if [[ " $* " == *" --visible "* && -z "${zox_tst_gpu_seconds:-}" ]]; then
    export zox_tst_gpu_seconds=20
fi
output=$(bash bsh/tst.sh gpu_light_stress "$@" 2>&1)
code=$?
echo "${output}" | grep -a "gpu_light_stress\|stress last frame\|PASSED\|FAIL\|exit \[\|build failed" | sed 's/^.*\[tst\] //'
if [[ -f tst/gpu_light_stress.ppm ]]; then
    if command -v convert > /dev/null; then
        convert tst/gpu_light_stress.ppm tst/gpu_light_stress.png && echo "> tst/gpu_light_stress.png"
    else
        echo "> tst/gpu_light_stress.ppm"
    fi
fi
exit ${code}
