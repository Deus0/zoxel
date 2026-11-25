#!/usr/bin/env bash
# end_game_cleanup x2 (two pre-existing failure modes: the +6 leak, and the settle stall)
cd "$(dirname "$0")/../.."
export zox_tst_timeout=300
: > tst/end_game_fix.txt
for r in 1 2; do
    echo "=== RUN ${r} ==="
    bash bsh/tst.sh end_game_cleanup
    code=$?
    if grep -aq "\[tst\] \[end_game_cleanup\] PASSED" tst/end_game_cleanup.log 2>/dev/null; then
        s="PASSED"
    else
        s="FAILED"
    fi
    echo "=== RESULT: ${s} (exit ${code}) ==="
    echo "run${r}|${s}|${code}" >> tst/end_game_fix.txt
done
echo "=== SUMMARY ==="
cat tst/end_game_fix.txt
