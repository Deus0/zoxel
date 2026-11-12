#!/usr/bin/env bash
# Summarises a ZOX_SAMPLE file (src/_/flecs/fun/sampler.c): where slow frames spend their time
#   ZOX_SAMPLE=/tmp/samples.txt ./zoxel.bin --tst perf_fly ...
#   bash bsh/sample_report.sh /tmp/samples.txt bin/zoxel_linux_x64_opengl_sdl/zoxel.bin [min_samples_a_frame=15]
#   - a sample is 2ms, so frames with min_samples or more are the slow ones (15 = 30ms)
#   - inclusive: % of slow frame samples a function is anywhere in the stack, leaf: % where it's running
#   - needs only bash, nm, sort and awk
samples="${1:?samples file}"
binary="${2:?binary}"
min_samples="${3:-15}"
symbols="$(mktemp)"
trap 'rm -f "${symbols}"' EXIT
nm "${binary}" | awk '$2 ~ /^[tTwW]$/ { print $1, $3 }' | sort > "${symbols}"
awk -v min="${min_samples}" '
function hex(s,    i, c, n) {
    sub(/^0x/, "", s); n = 0
    for (i = 1; i <= length(s); i++) {
        c = index("0123456789abcdef", tolower(substr(s, i, 1))) - 1
        n = n * 16 + c
    }
    return n
}
function name_of(a,    lo, hi, mid) {
    lo = 1; hi = nsyms
    if (a < addrs[1]) return "?"
    while (lo < hi) {
        mid = int((lo + hi + 1) / 2)
        if (addrs[mid] <= a) lo = mid; else hi = mid - 1
    }
    return names[lo]
}
FNR == 1 { pass++ }
pass == 1 { nsyms++; addrs[nsyms] = hex($1); names[nsyms] = $2; if ($2 == "zox_sample_handler") handler = addrs[nsyms]; next }
pass == 2 && FNR == 1 { next }
pass == 2 { frame_samples[$2]++; frames_seen[$2] = 1; next }
pass == 3 && FNR == 1 { base = hex($2) - handler; next }
{
    if (frame_samples[$2] < min) next
    total++
    delete seen
    # skips the handler and the signal frame
    for (i = 5; i <= NF; i++) {
        f = name_of(hex($i) - base)
        if (i == 5) leaf[f]++
        if (!(f in seen)) { seen[f] = 1; inclusive[f]++ }
    }
}
END {
    for (f in frames_seen) { frames++; if (frame_samples[f] >= min) slow++ }
    printf "frames [%d] slow [%d] samples in slow frames [%d]\n", frames, slow, total
    if (!total) exit
    print "--- inclusive"
    for (f in inclusive) printf "%5.1f%% %s\n", 100 * inclusive[f] / total, f | "sort -rn | head -30"
    close("sort -rn | head -30")
    print "--- leaf"
    for (f in leaf) printf "%5.1f%% %s\n", 100 * leaf[f] / total, f | "sort -rn | head -15"
}' "${symbols}" "${samples}" "${samples}"
