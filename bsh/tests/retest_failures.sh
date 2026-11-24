#!/usr/bin/env bash
# re-runs the full-run failures solo, to separate real bugs from load flakiness
cd "$(dirname "$0")/../.." || exit 1
export zox_tst_timeout=300
tests=(lint_systems gpu_restore camera_filter particles_render sky_clouds realm_pause feet_planted end_game_cleanup navigator walk_bob editor xr_body xr_navigator)
summary="tst/retest_failures.txt"
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
done
echo "=== SOLO RETEST SUMMARY ==="
cat "${summary}"
exit "${failed}"
