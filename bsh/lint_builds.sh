#!/usr/bin/env bash
# Lint: the game compiles in the builds the makefile makes, not just the debug one tests use
#  - timings (make prerelease, make profiler): timed systems, zox_sys_on_begin uses core's get_time_ms
#  - release without logs (make build, packages): no names, no log only code
#  - each builds into its own bin/lint_<name> (build.sh --out), so it doesn't touch the tests' binary
#  - run by bash bsh/tst.sh lint_builds (and all), takes a few minutes
cd "$(dirname "$0")/.." || exit 2
failed=0
builds=0
check() {
    local name="$1"; shift
    builds=$((builds + 1))
    if bash bsh/build.sh zoxel "$@" --out "lint_${name}" > "tst/lint_build_${name}.log" 2>&1; then
        echo "+ [${name}] builds ($*)"
    else
        failed=$((failed + 1))
        echo "!! [${name}] doesn't build ($*), tst/lint_build_${name}.log:"
        grep -m 10 -E "error" "tst/lint_build_${name}.log"
    fi
}
mkdir -p tst
check prerelease --logs --timings --release
check release --release
echo "[tst] [lint_builds] $([[ ${failed} == 0 ]] && echo PASSED || echo FAILED) [${builds}] builds, [${failed}] failed"
[[ ${failed} == 0 ]]
