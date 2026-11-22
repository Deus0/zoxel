#!/usr/bin/env bash
# Code stats: src's size, per module (src/<module>), so it doesn't bloat unnoticed
#   bash bsh/code_stats.sh            the stats, and each module's change since the baseline (doc/data/code_stats.txt)
#   bash bsh/code_stats.sh --save     the stats saved as the new baseline (after a deliberate change, commit it)
#   also: bash bsh/tst.sh code_stats
# - lines: code (not blank, not only a comment), comments, blank; tests (a module's tst folders) counted apart
# - the biggest files, and modules over their baseline by more than 5% (and 50 lines) flagged
# - writes tst/code_stats.txt; exit 0, it only reports
set -uo pipefail
cd "$(dirname "$0")/.." || exit 1
python3 -I - "$@" <<'PY'
import os, sys, re
save = "--save" in sys.argv
baseline_path = "doc/data/code_stats.txt"
def count(path):
    code = comment = blank = 0
    in_block = False
    with open(path, errors="replace") as f:
        for line in f:
            s = line.strip()
            if not s:
                blank += 1
                continue
            if in_block:
                comment += 1
                if "*/" in s:
                    in_block = False
                continue
            if s.startswith("//"):
                comment += 1
            elif s.startswith("/*"):
                comment += 1
                in_block = "*/" not in s
            else:
                code += 1
    return code, comment, blank
modules = {}
files = []
for root, dirs, names in os.walk("src"):
    for n in names:
        if not n.endswith((".c", ".h")):
            continue
        p = os.path.join(root, n)
        parts = p.split(os.sep)
        module = parts[1] if len(parts) > 2 else "(root)"
        test = "tst" in parts[2:-1]
        c, m, b = count(p)
        d = modules.setdefault(module, {"code": 0, "comment": 0, "blank": 0, "tests": 0, "files": 0})
        if test:
            d["tests"] += c
        else:
            d["code"] += c
            d["comment"] += m
            d["blank"] += b
        d["files"] += 1
        files.append((c + m, p))
baseline = {}
if os.path.exists(baseline_path):
    for line in open(baseline_path):
        f = line.split()
        if len(f) >= 3 and f[0] != "module" and not line.startswith("#"):
            baseline[f[0]] = (int(f[1]), int(f[2]))
total = {k: sum(d[k] for d in modules.values()) for k in ("code", "comment", "blank", "tests", "files")}
out = []
out.append(f"# src: {total['code']} lines of code, {total['comment']} of comments, {total['blank']} blank, {total['tests']} of tests, {total['files']} files")
out.append(f"{'module':<16} {'code':>7} {'tests':>7} {'comment':>8} {'files':>6} {'change':>8}")
flagged = []
for name, d in sorted(modules.items(), key=lambda kv: -kv[1]["code"]):
    change = ""
    if name in baseline:
        delta = d["code"] - baseline[name][0]
        change = f"{delta:+d}" if delta else "0"
        if delta > 50 and delta > baseline[name][0] * 0.05:
            flagged.append(f"{name} +{delta} code lines ({delta * 100 // max(1, baseline[name][0])}%)")
    elif baseline:
        change = "new"
    out.append(f"{name:<16} {d['code']:>7} {d['tests']:>7} {d['comment']:>8} {d['files']:>6} {change:>8}")
if baseline:
    base_total = sum(v[0] for v in baseline.values())
    out.append(f"total code {total['code']} ({total['code'] - base_total:+d} since the baseline)")
out.append("")
out.append("biggest files (code and comments):")
for n, p in sorted(files, reverse=True)[:12]:
    out.append(f"  {n:>6} {p}")
if flagged:
    out.append("")
    out.append("grown over 5% since the baseline:")
    out += ["  " + f for f in flagged]
text = "\n".join(out)
print(text)
os.makedirs("tst", exist_ok=True)
open("tst/code_stats.txt", "w").write(text + "\n")
if save:
    with open(baseline_path, "w") as f:
        f.write("# the code stats baseline (bash bsh/code_stats.sh --save): module, code lines, test lines\n")
        f.write("module code tests\n")
        for name, d in sorted(modules.items()):
            f.write(f"{name} {d['code']} {d['tests']}\n")
    print(f"\nsaved the baseline: {baseline_path}")
PY
