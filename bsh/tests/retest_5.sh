#!/usr/bin/env bash
# re-runs the five real failures solo, with a fresh build (tree moved: rebase + wip block_break test)
cd "$(dirname "$0")/../.."
export zox_tst_timeout=300
tests=(camera_filter end_game_cleanup walk_bob editor xr_body)
summary="tst/retest_5.txt"
: > "${summary}"
for t in "${tests[@]}"; do
    echo "=== TEST: ${t} ==="
    start=$(date +%s)
    bash bsh/tst.sh "${t}"
    code=$?
    end=$(date +%s)
    if grep -aq "\[tst\] \[${t}\] PASSED" "tst/${t}.log" 2>/dev/null; then r="PASSED"; else r="FAILED"; fi
    echo "=== RESULT: ${t} ${r} (exit ${code}, $((end - start))s) ==="
    echo "${t}|${r}|${code}|$((end - start))" >> "${summary}"
done
echo "=== RETEST-5 SUMMARY ==="
cat "${summary}"
