#!/usr/bin/env bash
# Looks at gpu meshing: vox models animated every frame, meshed by a compute shader and drawn from the gpu
#   (tst gpu_mesh_views, doc/tec/gpu_meshing.md), saves tst/gpu_mesh_views.png and logs models meshed a second
#   bash bsh/gpu_mesh.sh                        9 chickens, 6 seconds, software rendering (as tests)
#   bash bsh/gpu_mesh.sh --visible              watch it in a window on the real gpu
#   zox_tst_gpu_models=36 bash bsh/gpu_mesh.sh  more models, to see how far it goes (up to 64)
#   zox_tst_vox=player zox_tst_gpu_seconds=20 bash bsh/gpu_mesh.sh --nobuild   another vox file (res/voxes), longer
set -uo pipefail
output=$(bash bsh/tst.sh gpu_mesh_views "$@" 2>&1)
code=$?
echo "${output}" | grep -a "gpu_mesh_views\|view \[\|PASSED\|FAIL\|exit \[\|build failed" | sed 's/^.*\[tst\] //'
if [[ -f tst/gpu_mesh_views.ppm ]]; then
    if command -v convert > /dev/null; then
        convert tst/gpu_mesh_views.ppm tst/gpu_mesh_views.png && echo "> tst/gpu_mesh_views.png"
    else
        echo "> tst/gpu_mesh_views.ppm"
    fi
fi
exit ${code}
