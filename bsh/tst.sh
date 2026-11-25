#!/usr/bin/env bash
# Runs automated tests headless, exit code 0 = all passed
#   bash bsh/tst.sh all [--nobuild] [extra game args]     (make tests)
#   bash bsh/tst.sh <name> [--nobuild] [extra game args]  (make test name=<name>)
#   bash bsh/tst.sh list
#   bash bsh/tst.sh code_stats [--save]   (src's size per module: bsh/code_stats.sh)
#   --repeat <n>: runs the test(s) n times, for intermittent bugs (i.e. player_spawn --repeat 5)
#   --gdb: runs under gdb, prints a backtrace on crash
#   --visible: shows the window (no --background) on the real gpu, i.e. to compare gpu memory
#   --gpu: hidden, but on the real gpu (mali) inside the desktop session
#   default is software rendering (mesa llvmpipe, sdl offscreen, no wayland window), since
#     mali faults in a test run once crashed gnome-shell and logged the user out (2026-11-11)
#     still opengl es 3.0 (--gles), the same render path the game takes on mali
#   --stress: full terrain stream distance, tests default to --tiny terrain
#   perf tests (perf_tests below) run alone at far 8 (their measuring scale), in a full run tiny (a smoke run)
#   --release: an optimised build with the tests in it (bin/tst_release), for performance numbers (perf_fly,
#     vox_instances_stress): debug builds are -O0, their cpu times are many times a player's
#   --readme: the aesthetic tests (aesthetic_town) update the readme's screenshots in doc/screenshot/, without it
#     they stay in tst/aesthetic/ (every full run rewrote them, the same shots a few pixels apart)
#   --keepsaves: saves to the real save folder, tests default to --nosaves (a /tmp folder, deleted after)
#   --vulkan: the vulkan build (bin/tst_vulkan), on lavapipe (mesa's software vulkan) unless --gpu
# Tests are added with zox_tst_add in a module's tst folder (see src/_/tst/_.c)
# Safety: low priority, memory capped and killed on timeout, so a bad run can't freeze the pc
#   override with zox_tst_timeout=<seconds> zox_tst_memory=<size> zox_tst_log_max=<bytes>
#   also killed if the machine's MemAvailable drops below zox_tst_mem_floor (KB, default 1GB of the 8GB board),
#   since gpu driver memory isn't under the cgroup cap; trace in tst/<name>.mem
# Fail fast: killed once its log passes zox_tst_log_max (log spam), and the game itself
#   fails a test after 100 logged errors (--tst-errors <n>)
set -uo pipefail

name="${1:-all}"
shift || true
# code_stats: src's size per module, no game run (bsh/code_stats.sh)
if [[ "${name}" == "code_stats" ]]; then
    exec bash bsh/code_stats.sh "$@"
fi
build=1
gdb=0
stress=0
keepsaves=0
visible=0
gpu=0
release=0
repeat=1
readme=0
vulkan=0
args=()
user_size=0
next_is_repeat=0
for arg in "$@"; do
    if [[ ${next_is_repeat} == "1" ]]; then
        repeat="${arg}"
        next_is_repeat=0
    elif [[ "${arg}" == "--repeat" ]]; then
        next_is_repeat=1
    elif [[ "${arg}" == "--nobuild" ]]; then
        build=0
    elif [[ "${arg}" == "--readme" ]]; then
        readme=1
    elif [[ "${arg}" == "--gdb" ]]; then
        gdb=1
    elif [[ "${arg}" == "--stress" ]]; then
        stress=1
    elif [[ "${arg}" == "--visible" ]]; then
        visible=1
        gpu=1
    elif [[ "${arg}" == "--gpu" ]]; then
        gpu=1
    elif [[ "${arg}" == "--release" ]]; then
        release=1
    elif [[ "${arg}" == "--keepsaves" ]]; then
        keepsaves=1
    elif [[ "${arg}" == "--vulkan" ]]; then
        vulkan=1
    else
        args+=("${arg}")
        # a size of terrain asked for: it wins over a perf test's own scale
        [[ "${arg}" == "--tiny" || "${arg}" == "--medium" || "${arg}" == "--large" || "${arg}" == "--far" ]] && user_size=1
    fi
done

# windows host (git bash): no mesa or egl to render in software, so tests run hidden on the real gpu (--gpu), and
#   the game is an .exe; no systemd or pgrep there, so the memory cap and its guards are off
bin_ext="bin"
windows=0
case "$(uname -s)" in
    MINGW*|MSYS*|CYGWIN*) windows=1; gpu=1; bin_ext="exe" ;;
esac
# is a game's pid running: windows games have windows pids, which kill -0 (git bash's own) doesn't see
pid_alive() {
    if [[ ${windows} == "1" ]]; then
        tasklist //FI "PID eq ${1}" //NH 2> /dev/null | grep -q " ${1} "
    else
        kill -0 "${1}" 2> /dev/null
    fi
}
timeout_seconds="${zox_tst_timeout:-100}"
memory_max="${zox_tst_memory:-4G}"   # 8GB boards, often idle while tests run
log_max="${zox_tst_log_max:-20000000}"  # 20MB, a healthy test logs a few KB
# system wide guard: gpu driver memory (mali) isn't charged to the cgroup cap, so also
# kill the run when the whole machine's MemAvailable drops below this (in KB)
mem_floor_kb="${zox_tst_mem_floor:-1000000}"
# small terrain unless stress testing it, streaming is the heaviest part of a run
if [[ ${stress} == "0" ]]; then
    args+=("--tiny")
fi
# saves go to /tmp/zoxel_nosaves_<pid> (on windows %TEMP%, the same folder), so test runs don't fill the real save folder
if [[ ${keepsaves} == "0" ]]; then
    args+=("--nosaves")
fi
mode_args=(--background)
[[ ${visible} == "1" ]] && mode_args=()
# software rendering: no mali context and no compositor involved, so a driver fault can't
# take the desktop down. glvnd libs ahead of the mali folder in ld.so.conf, mesa as vendor
# the egl device for mesa software: 2 on the arm board (0,1 are the drm cards, mesa segfaults on them), 0 on x86
# (with mesa the only vendor it is the one device sdl sees; 2 there fails "Invalid EGL device is requested")
soft_env=()
arm=0
arm_libs=""
[[ "$(uname -m)" == aarch64 ]] && arm=1 && arm_libs=/usr/lib/aarch64-linux-gnu
if [[ ${gpu} == "0" ]]; then
    soft_env=(env
        SDL_VIDEODRIVER=offscreen
        SDL_HINT_EGL_DEVICE="${zox_tst_egl_device:-$(( arm ? 2 : 0 ))}"
        LD_LIBRARY_PATH="${arm_libs}"
        __EGL_VENDOR_LIBRARY_FILENAMES=/usr/share/glvnd/egl_vendor.d/50_mesa.json
        LIBGL_ALWAYS_SOFTWARE=1
        GALLIUM_DRIVER=llvmpipe
        EGL_PLATFORM=surfaceless)
    # vulkan: lavapipe, mesa's software vulkan (vulkan-swrast)
    if [[ ${vulkan} == "1" ]]; then
        lavapipe=$(ls /usr/share/vulkan/icd.d/lvp_icd*.json 2> /dev/null | head -1)
        soft_env+=(VK_DRIVER_FILES="${lavapipe}" VK_ICD_FILENAMES="${lavapipe}")
    fi
    # llvmpipe streams terrain ~2x slower than mali (stream_npcs spawn 52s vs 20s)
    mode_args+=(--gles --tst-timescale 2)
    [[ -z "${zox_tst_timeout:-}" ]] && timeout_seconds=200
fi
root="$(pwd)"
# tst/: logs, and the files tests write to look at (screenshots, wavs), gitignored
#   the game finds it from zox_tst_dir (src/_/tst/_.c zox_tst_path); set, i.e. zox_tst_dir=tst/mine, logs go there
#   and the build to bin/tst_mine/
log_dir="${zox_tst_dir:-${root}/tst}"
[[ "${log_dir}" != /* ]] && log_dir="${root}/${log_dir}"
# with zox_tst_dir set the build goes to bin/tst_<dir>/, zox_tst_out names it
out_name="${zox_tst_out:-}"
[[ -z "${out_name}" && -n "${zox_tst_dir:-}" ]] && out_name="tst_$(basename "${log_dir}")"
# the vulkan build has its own folder, so it's never taken for the opengl one
[[ ${vulkan} == "1" && -z "${out_name}" ]] && out_name="tst_vulkan"
out_args=()
[[ -n "${out_name}" ]] && out_args=(--out "${out_name}")
[[ ${vulkan} == "1" ]] && out_args+=(--vulkan)
mkdir -p "${log_dir}"
export zox_tst_dir="${log_dir}"
# lint_* tests are scripts (bsh/<name>.sh), not the game: conventions, i.e. lint_systems
lints=(lint_systems lint_builds)
run_lint() {
    bash "${root}/bsh/${1}.sh" > "${log_dir}/${1}.log" 2>&1
    local code=$?
    grep -a "\[tst\] \[${1}\]" "${log_dir}/${1}.log" | sed 's/^/    /'
    [[ ${code} != 0 ]] && head -20 "${log_dir}/${1}.log" | grep -v "\[tst\]" | sed 's/^/    | /'
    return ${code}
}

if [[ "${name}" == lint_* ]]; then
    run_lint "${name}"
    exit $?
fi

mkdir -p "${log_dir}"

# the release build with tests, its own folder for vulkan
release_name="tst_release"
release_args=()
[[ ${vulkan} == "1" ]] && release_name="tst_release_vulkan" && release_args=(--vulkan)
if [[ ${build} == "1" && ${release} == "1" ]]; then
    echo "> building release, with the tests"
    if ! (ulimit -v 4500000; zox_extra_dflags="-Dzox_tests" nice -n 19 bash bsh/build.sh zoxel --release --logs \
        --out "${release_name}" "${release_args[@]}" > "${log_dir}/build.log" 2>&1); then
        grep -m 20 "error" "${log_dir}/build.log"
        echo "!! build failed, see [${log_dir#${root}/}/build.log]"
        exit 2
    fi
elif [[ ${build} == "1" ]]; then
    echo "> building debug"
    # cap the compiler too, it's a single big translation unit
    if ! (ulimit -v 4500000; nice -n 19 bash bsh/build.sh zoxel --debug --logs "${out_args[@]}" > "${log_dir}/build.log" 2>&1); then
        grep -m 20 "error" "${log_dir}/build.log"
        echo "!! build failed, see [${log_dir#${root}/}/build.log]"
        exit 2
    fi
fi

bin=$(ls -t bin/zoxel_*_opengl_*_dev/zoxel-dev.${bin_ext} 2>/dev/null | head -1)
[[ -n "${out_name}" ]] && bin="bin/${out_name}/${out_name}.${bin_ext}" && [[ ! -f "${bin}" ]] && bin=""
[[ ${release} == "1" ]] && bin="bin/${release_name}/${release_name}.${bin_ext}"
if [[ -z "${bin}" ]]; then
    echo "!! no dev binary found in [bin/zoxel_*_dev]"
    exit 2
fi
bin_dir="$(dirname "${bin}")"
bin_name="$(basename "${bin}")"

cap=()
if command -v systemd-run > /dev/null && systemd-run --user --scope -q true 2> /dev/null; then
    cap=(systemd-run --user --scope -q -p "MemoryMax=${memory_max}" -p MemorySwapMax=0)
fi

# runs the game with --tst <name>, output to <log>, returns its exit code
# tests that need more terrain than --tiny (i.e. far lods to compare), the later --medium wins
medium_tests=(lod_chunk_lights lod_swap stream_seams chunk_mesh_flicker)
# tests that need the game's far lods (far chunks generated coarse, grown finer as the stream comes: lights cleared)
far_tests=(lod_light_flash)
# performance tests: alone they run at far 8 (their measuring scale: not --tiny, whose numbers aren't the game's, the
#   far lods are most of the terrain; far 16 is too slow for a debug build's step limits, pass it with --release), in a
#   full run (all) a quick smoke run at tiny; a size passed (--far 16) wins
perf_tests=(perf_terrain perf_stream perf_fly tables_stream perf_idle)
# tests that screenshot the world, the hud hidden
nohud_tests=(sky_clouds head_camera town_view held_item held_block held_view eat_swing)
# tests that take the readme's screenshots: a far, tall world at 1080p, little fog (to see the mountains), no hud or labels, the same seed (zox_tst_seed
#   overrides it), more time to stream, their shots converted to doc/screenshot/ after with --readme
aesthetic_tests=(aesthetic_town aesthetic_particles)
# tests that compare pictures across a change: the same world every run (seed 666, zox_tst_seed overrides it), a
#   random one gave them other weather and effects to differ by
seeded_tests=(gpu_restore_game)
run_game() {
    local tst="$1"
    local log="$2"
    local debugger=()
    local test_args=()
    # a size passed (--far, --large, --tiny) wins: --medium after it set lod far 6 over a --far 16
    [[ " ${medium_tests[*]} " == *" ${tst} "* && " ${args[*]} " != *" --far "* && " ${args[*]} " != *" --large "* ]] &&         test_args=(--medium)
    [[ " ${far_tests[*]} " == *" ${tst} "* && " ${args[*]} " != *" --far "* ]] && test_args+=(--far 16)
    local perf_scale=0
    [[ " ${perf_tests[*]} " == *" ${tst} "* && "${name}" != "all" && ${user_size} == "0" ]] && perf_scale=1
    [[ ${perf_scale} == "1" ]] && test_args+=(--far 8)
    [[ " ${nohud_tests[*]} " == *" ${tst} "* ]] && test_args+=(--nohud)
    local seconds="${timeout_seconds}"
    # far lods load and stream a while: room for them
    [[ " ${far_tests[*]} " == *" ${tst} "* || ${perf_scale} == "1" ]] && seconds=$(( timeout_seconds > 600 ? timeout_seconds : 600 ))
    local seed_env=()
    [[ " ${seeded_tests[*]} " == *" ${tst} "* ]] && seed_env=(env "zox_tst_seed=${zox_tst_seed:-666}")
    if [[ " ${aesthetic_tests[*]} " == *" ${tst} "* ]]; then
        test_args+=(--far 16 --vertical 8 --fog 0.025 --1080p --nohud --nouis --tst-timescale 3)
        seed_env=(env "zox_tst_seed=${zox_tst_seed:-375758414}")
        seconds=$(( timeout_seconds > 300 ? timeout_seconds : 300 ))
    fi
    if [[ ${gdb} == "1" ]]; then
        debugger=(gdb -q -batch -ex "handle SIGPIPE nostop" -ex run -ex bt --args)
    fi
    # exec, so $! is timeout's pid, and killing it takes the game down with it
    (
        cd "${bin_dir}" &&
        exec timeout -k 5 "${seconds}" "${cap[@]}" "${seed_env[@]}" "${soft_env[@]}" nice -n 10 \
            "${debugger[@]}" "./${bin_name}" --tst "${tst}" --autostart "${mode_args[@]}" "${args[@]}" "${test_args[@]}" \
            > "${log}" 2>&1
    ) &
    local pid=$!
    local spammed=0
    local lowmem=0
    local onmali=0
    local printed=0
    local ticks=0
    local mem_log="${log%.log}.mem"
    : > "${mem_log}"
    # this run's game, a descendant of the timeout (gdb, systemd-run and nice may sit between): never another
    #   process of the same name, i.e. the game the user is playing (pgrep -n by name killed theirs, 2026-11-14)
    run_game_pid() {
        local parents=("${pid}")
        local depth
        for depth in 1 2 3 4 5; do
            local children=()
            local parent
            for parent in "${parents[@]}"; do
                local child
                for child in $(pgrep -P "${parent}" 2> /dev/null); do
                    if [[ "$(cat /proc/${child}/comm 2> /dev/null)" == "${bin_name:0:15}" ]]; then
                        echo "${child}"
                        return
                    fi
                    children+=("${child}")
                done
            done
            [[ ${#children[@]} -eq 0 ]] && return
            parents=("${children[@]}")
        done
    }
    # kills this run's game and its timeout, nothing else
    kill_run_game() {
        local game_pid
        game_pid=$(run_game_pid)
        [[ -n "${game_pid}" ]] && kill -KILL "${game_pid}" 2> /dev/null
        kill -KILL "${pid}" 2> /dev/null
    }
    # prints new [tst] lines while it runs, so a slow test doesn't look hung
    print_tst_lines() {
        [[ "${tst}" == "list" ]] && return
        local lines
        lines=$(grep -a "\[tst\]" "${log}" 2> /dev/null | tail -n +$((printed + 1)))
        if [[ -n "${lines}" ]]; then
            echo "${lines}" | sed 's/^/    /'
            printed=$((printed + $(echo "${lines}" | wc -l)))
        fi
    }
    while kill -0 "${pid}" 2> /dev/null; do
        print_tst_lines
        if [[ $(stat -c %s "${log}" 2> /dev/null || echo 0) -gt ${log_max} ]]; then
            spammed=1
            kill -TERM "${pid}" 2> /dev/null
            sleep 2
            kill -KILL "${pid}" 2> /dev/null
            break
        fi
        local available
        available=$(awk '/^MemAvailable:/ {print $2}' /proc/meminfo)
        # trace to disk every 2s and sync it, so a freeze still leaves evidence
        if (( ticks % 4 == 0 )); then
            echo "$(date +%H:%M:%S) available_kb ${available} $(awk '/^(RssAnon|RssFile|RssShmem):/ {printf "%s %s ", $1, $2}' /proc/$(run_game_pid || true)/status 2> /dev/null)" >> "${mem_log}"
            sync "${mem_log}" 2> /dev/null
        fi
        ticks=$((ticks + 1))
        # software mode must never touch the mali device, kill it if it does
        if [[ ${gpu} == "0" ]]; then
            local game_pid
            game_pid=$(run_game_pid)
            if [[ -n "${game_pid}" ]] && ls -l /proc/${game_pid}/fd 2> /dev/null | grep -q "/dev/mali"; then
                onmali=1
                kill_run_game
                break
            fi
        fi
        if [[ -n "${available}" ]] && (( available < mem_floor_kb )); then
            lowmem=1
            kill_run_game
            break
        fi
        sleep 0.5
    done
    wait "${pid}"
    local code=$?
    print_tst_lines
    # only the folders of games no longer running (named by the game's pid): another test running at the same
    #   time lost its save folder mid game (its saves failed, end_game_cleanup's rounds came out different)
    local saves
    for saves in /tmp/zoxel_nosaves_*; do
        [[ -d "${saves}" ]] && ! pid_alive "${saves##*_}" && rm -rf "${saves}"
    done
    if [[ ${onmali} == "1" ]]; then
        echo "!! software mode opened /dev/mali, killed (use --gpu for real gpu runs)" >> "${log}"
        return 127
    fi
    if [[ ${lowmem} == "1" ]]; then
        echo "!! system memory available dropped below ${mem_floor_kb}KB, killed (see ${mem_log})" >> "${log}"
        return 126
    fi
    if [[ ${spammed} == "1" ]]; then
        echo "!! log passed ${log_max} bytes, killed (log spam)" >> "${log}"
        return 125
    fi
    return ${code}
}

# the aesthetic tests' shots (tst/aesthetic/*.ppm) to doc/screenshot/*.jpg, half size
aesthetic_convert() {
    if ! command -v magick > /dev/null; then
        echo "    !! no magick (imagemagick), the shots stay in tst/aesthetic/"
        return
    fi
    mkdir -p "${root}/doc/screenshot"
    local ppm
    for ppm in "${log_dir}"/aesthetic/*.ppm; do
        [[ -f "${ppm}" ]] || continue
        local jpg="${root}/doc/screenshot/$(basename "${ppm}" .ppm).jpg"
        magick "${ppm}" -resize 50% -quality 88 "${jpg}" && echo "    + ${jpg#${root}/}"
    done
    # gifs (aesthetic_particles) to doc/gifs/, 640 wide, their frames optimised: a readme sized file
    local gif
    for gif in "${log_dir}"/aesthetic/*.gif; do
        [[ -f "${gif}" ]] || continue
        mkdir -p "${root}/doc/gifs"
        local out="${root}/doc/gifs/$(basename "${gif}")"
        magick "${gif}" -coalesce -resize 640x -layers Optimize "${out}" && echo "    + ${out#${root}/}"
    done
}

# runs one test, prints its [tst] lines, keeps the log if it failed
run_tst() {
    local tst="$1"
    local log="${log_dir}/${tst}.log"
    echo "> tst [${tst}]"
    run_game "${tst}" "${log}"
    local code=$?
    if [[ ${gdb} == "1" ]]; then
        # gdb's exit code isn't the game's
        code=1
        grep -aq "received signal" "${log}" && code=139
    fi
    # a crash can still exit 0 (segfault handler, or gdb), only a logged pass counts
    if [[ ${code} == "0" || ${gdb} == "1" ]] && ! grep -aq "\[tst\] \[${tst}\] PASSED" "${log}"; then
        [[ ${code} == "0" ]] && code=3
    elif [[ ${gdb} == "1" ]]; then
        code=0
    fi
    if grep -aq "received signal" "${log}"; then
        grep -a -A30 "received signal" "${log}" | grep -a "^#\|signal" | head -25 | sed 's/^/    /'
    fi
    # the game's segfault handler logs a backtrace (glibc), resolve it to functions and lines
    if grep -aq "Segfault Detected" "${log}" && command -v addr2line > /dev/null; then
        echo "    !! segfault, backtrace:"
        grep -aoP "${bin_name}\(\+\K0x[0-9a-f]+" "${log}" | head -25 | \
            xargs -r addr2line -f -C -e "${bin}" | paste - - | sed 's/^/    /'
    fi
    if [[ ${code} == "0" && ${readme} == "1" && " ${aesthetic_tests[*]} " == *" ${tst} "* ]]; then
        aesthetic_convert
    fi
    if [[ ${code} == "125" ]]; then
        echo "    !! killed, log passed ${log_max} bytes (log spam)"
        # the spam itself is the useful part, show what repeated most
        grep -a "ERROR" "${log}" | sed 's/^.*\] \[ERROR\]/[ERROR]/' | sort | uniq -c | sort -rn | head -3 | sed 's/^/    /'
    elif [[ ${code} == "126" ]]; then
        echo "    !! killed, system memory ran low (gpu/driver memory isn't under the cgroup cap)"
        tail -n 3 "${log_dir}/${tst}.mem" | sed 's/^/    /'
    elif [[ ${code} == "127" ]]; then
        echo "    !! killed, software mode opened /dev/mali"
    elif [[ ${code} == "124" || ${code} == "137" ]]; then
        echo "    !! killed after ${timeout_seconds}s (hung, or never reached its own timeout)"
    fi
    if [[ ${code} != "0" ]]; then
        # keep failed logs, the next run overwrites ${log}
        local failed_log="${log_dir}/${tst}_failed_$(date +%H%M%S).log"
        cp "${log}" "${failed_log}"
        tail -n 5 "${failed_log}" | sed 's/^/    | /'
        echo "    exit [${code}], log [${failed_log#${root}/}]"
    fi
    return ${code}
}

list_tsts() {
    run_game list "${log_dir}/list.log"
    grep -aoP "\[tst\] test: \K\S+" "${log_dir}/list.log"
}

if [[ "${name}" == "list" ]]; then
    list_tsts
    exit 0
fi

if ! [[ "${repeat}" =~ ^[1-9][0-9]*$ ]]; then
    echo "!! --repeat needs a count above 0, got [${repeat}]"
    exit 2
fi

if [[ "${name}" != "all" ]]; then
    if [[ ${repeat} == "1" ]]; then
        run_tst "${name}"
        exit $?
    fi
    tsts=("${name}")
else
    tsts=("${lints[@]}" $(list_tsts))
fi
if [[ ${#tsts[@]} == 0 ]]; then
    echo "!! no tests found, see [tst/list.log]"
    exit 2
fi
runs=$((${#tsts[@]} * repeat))
echo "> running [${#tsts[@]}] tests x [${repeat}] (timeout ${timeout_seconds}s, memory ${memory_max})"
passed=()
failed=()
for ((r = 1; r <= repeat; r++)); do
    [[ ${repeat} != "1" ]] && echo "> repeat [${r}/${repeat}]"
    for tst in "${tsts[@]}"; do
        if [[ "${tst}" == lint_* ]]; then
            run_lint "${tst}" && passed+=("${tst}") || failed+=("${tst}")
            continue
        fi
        if run_tst "${tst}"; then
            passed+=("${tst}")
        else
            failed+=("${tst}")
        fi
    done
done
echo ""
echo "> passed [${#passed[@]}/${runs}]"
if [[ ${#failed[@]} != 0 ]]; then
    echo "!! failed: ${failed[*]}"
    exit 1
fi
exit 0
