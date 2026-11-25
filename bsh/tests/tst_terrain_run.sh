#!/usr/bin/env bash
# runs the terrain tests codex astra was running, one by one, prints a summary line each
cd "$(dirname "$0")/../.." || exit 1
export zox_tst_timeout=300   # headroom over the 240s in-game timeouts; far tests take 600 automatically
tests=(lod_refine_border stream_seams chunk_mesh_flicker lod_swap lod_chunk_lights  vegetation_chunks lod_light_flash)
summary="tst/terrain_summary.txt"
mkdir -p tst || exit 1
: > "${summary}" || exit 1
failed=0
for t in "${tests[@]}"; do
    echo "=== TEST: ${t} ==="
    start=$(date +%s)
    bash bsh/tst.sh "${t}" --nobuild
    code=$?
    end=$(date +%s)
    if (( code == 0 )); then
        r=PASSED
    else
        r=FAILED
        failed=1
    fi
    echo "=== RESULT: ${t} ${r} (exit ${code}, $((end - start))s) ==="
    echo "${t}|${r}|${code}|$((end - start))" >> "${summary}"
    echo "=== SUMMARY SO FAR ==="; cat "${summary}"
done
echo "=== ALL DONE ==="
exit "${failed}"
