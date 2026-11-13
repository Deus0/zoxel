#!/usr/bin/env bash
# Lint: systems follow the conventions (doc/coding_standards.md), run by bash bsh/tst.sh lint_systems (and all)
#  - names are snake_case, i.e. chunk_sides_system (systems named by macros from a component, T##IncrementSystem,
#    are left alone, the preprocessor can't lowercase them)
#  - plain functions: void name_system(iter* it)
#  - timed: zox_sys_on_begin() first, zox_sys_on_end() at the end (systems' times, the profiler), except in
#    src/_ (the flecs layer is before core, where timed builds' get_time_ms and SystemProcessed are)
#  - commented out code is skipped, prints each problem as file:line, exit 1 if any
#  - needs only bash, grep and awk
cd "$(dirname "$0")/.." || exit 2
files=$(grep -rlE 'zoxd_system\(' src gam)
# shellcheck disable=SC2086
awk '
FNR == 1 { in_comment = 0; fn = "" }
{
    line = $0
    # blank out comments, /* */ can span lines
    out = ""
    while (length(line) > 0) {
        if (in_comment) {
            e = index(line, "*/")
            if (!e) { line = ""; break }
            line = substr(line, e + 2); in_comment = 0
        } else {
            b = index(line, "/*"); c = index(line, "//")
            if (c && (!b || c < b)) { out = out substr(line, 1, c - 1); line = ""; break }
            if (!b) { out = out line; line = ""; break }
            out = out substr(line, 1, b - 1); line = substr(line, b + 2); in_comment = 1
        }
    }
    code = out
    if (match(code, /^(static )?void [A-Za-z0-9_]+\(iter[^)]*\)[ \t]*\{/)) {
        decl = substr(code, RSTART, RLENGTH)
        sub(/^(static )?void /, "", decl)
        fn = decl; sub(/\(.*/, "", fn)
        args = decl; sub(/^[^(]*\(/, "", args); sub(/\).*/, "", args)
        fn_line = FNR; has_begin = 0; has_end = 0
    }
    if (fn != "" && code ~ /zox_sys_on_begin/) has_begin = 1
    if (fn != "" && code ~ /zox_sys_on_end/) has_end = 1
    while (match(code, /zoxd_system\([A-Za-z0-9_#]+\)/)) {
        name = substr(code, RSTART + 12, RLENGTH - 13)
        rest = substr(code, RSTART + RLENGTH)
        is_macro = (code ~ /\\[ \t]*$/)
        code = rest
        if (index(name, "##") || is_macro) continue
        systems++
        if (name !~ /^[a-z][a-z0-9_]*$/) {
            printf "%s:%d: [%s] isnt snake_case\n", FILENAME, FNR, name; problems++
        }
        if (fn == name) {
            if (args != "iter* it") { printf "%s:%d: [%s] takes (%s), not (iter* it)\n", FILENAME, fn_line, name, args; problems++ }
            if (!has_begin && FILENAME !~ /^src\/_\//) { printf "%s:%d: [%s] has no zox_sys_on_begin()\n", FILENAME, fn_line, name; problems++ }
            if (!has_end && FILENAME !~ /^src\/_\//) { printf "%s:%d: [%s] has no zox_sys_on_end()\n", FILENAME, fn_line, name; problems++ }
            fn = ""
        }
    }
}
END {
    printf "[tst] [lint_systems] %s [%d] systems, [%d] problems\n", (problems ? "FAILED" : "PASSED"), systems, problems
    exit (problems ? 1 : 0)
}' ${files}
