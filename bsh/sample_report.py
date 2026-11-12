#!/usr/bin/env python3
# Summarises a ZOX_SAMPLE file (src/_/flecs/fun/sampler.c): where slow frames spend their time
#   ZOX_SAMPLE=/tmp/samples.txt ./zoxel.bin --tst perf_fly ...
#   python3 bsh/sample_report.py /tmp/samples.txt bin/zoxel_linux_x64_opengl_sdl/zoxel.bin [min_samples_a_frame=15]
#   - a sample is 2ms, so frames with min_samples or more are the slow ones (15 = 30ms)
#   - inclusive: % of slow frame samples a function is anywhere in the stack, leaf: % where it's running
import bisect, collections, subprocess, sys
samples_path, binary = sys.argv[1], sys.argv[2]
min_samples = int(sys.argv[3]) if len(sys.argv) > 3 else 15
syms = []
for line in subprocess.run(["nm", binary], capture_output=True, text=True).stdout.splitlines():
    p = line.split()
    if len(p) == 3 and p[1] in "tTwW":
        syms.append((int(p[0], 16), p[2]))
syms.sort()
addrs = [a for a, _ in syms]
by_name = {n: a for a, n in syms}
lines = open(samples_path).read().splitlines()
base = int(lines[0].split()[1], 16) - by_name["zox_sample_handler"]
def name(x):
    i = bisect.bisect_right(addrs, int(x, 16) - base) - 1
    return syms[i][1] if i >= 0 else "?"
frames = collections.defaultdict(list)
for l in lines[1:]:
    p = l.split()
    frames[int(p[1])].append([name(x) for x in p[2:]][2:])  # skips the handler and the signal frame
slow = [s for s in frames.values() if len(s) >= min_samples]
count = sum(len(s) for s in slow)
print(f"frames [{len(frames)}] slow [{len(slow)}] samples in slow frames [{count}]")
inclusive, leaf = collections.Counter(), collections.Counter()
for frame in slow:
    for stack in frame:
        leaf[stack[0] if stack else "?"] += 1
        for f in set(stack):
            inclusive[f] += 1
print("--- inclusive")
for f, c in inclusive.most_common(30):
    print(f"{100 * c / max(count, 1):5.1f}% {f}")
print("--- leaf")
for f, c in leaf.most_common(15):
    print(f"{100 * c / max(count, 1):5.1f}% {f}")
