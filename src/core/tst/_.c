// Automated Tests
//  - run: bash bsh/tst.sh <name>  (or ./zoxel-dev.bin --tst <name> --autostart --background)
//  - a test registers with zox_tst_add(name, ready, check, timeout) in its module
//      ready: returns 1 once the world is in a state to check
//      check: uses zox_tst_check, failures are counted and logged
//  - the app exits once checked (exit code 0 pass, 1 fail), or fails on timeout
//  - list tests: --tst list, run all: bash bsh/tst.sh all (make tests)
//  - fails fast once [zox_tst_errors_max] errors are logged during the test (error spam)
//  - fails fast when its zox_tst_progress value doesn't change for [zox_tst_stall_seconds]
//  - logs "still waiting" every [zox_tst_heartbeat_seconds]

#include "profile.c"

typedef byte (*zox_tst_ready_fun)(ecs* world);
typedef void (*zox_tst_check_fun)(ecs* world);
typedef void (*zox_tst_setup_fun)();

typedef struct {
    const char* name;
    zox_tst_ready_fun ready;
    zox_tst_check_fun check;
    double timeout;
    zox_tst_setup_fun setup;    // optional, runs with the terminal arguments, before boot
    byte menus;                 // runs through the menus, ignoring --autostart
    byte exits;                 // passes when the app closes itself (the main loop ends cleanly)
} zox_tst;

#define zox_tsts_max 128
zox_tst zox_tsts[zox_tsts_max];
int zox_tsts_count = 0;
const char* zox_tst_name = NULL;   // set by --tst <name>
byte zox_tst_menus = 0;             // the running test wants menus, huds skips --autostart
zox_tst* zox_tst_active = NULL;
int zox_tst_fails = 0;
int zox_tst_checks = 0;
const char* zox_tst_waiting = ""; // set by ready functions, logged on timeout
// fail fast: errors logged since the test started, over this ends it (--tst-errors <n>)
uint zox_tst_errors_max = 100;
uint zox_tst_errors_start = 0;
// fail fast: a waiting test reports progress with zox_tst_progress, no change for this long fails it
double zox_tst_stall_seconds = 20;
// scales every test's timeout, i.e. software rendering runs slower (--tst-timescale <f>)
double zox_tst_timescale = 1;
double zox_tst_progress_value = 0;
double zox_tst_progress_time = 0;
byte zox_tst_progress_used = 0;
double zox_tst_progress_longest = 0;   // longest no progress wait, logged at the end to tune the stall
// logs what it's waiting for every few seconds, so a slow test doesn't look hung
double zox_tst_heartbeat_seconds = 5;
double zox_tst_started_time = 0;
double zox_tst_heartbeat_time = 0;

// called by ready functions with any number that changes while things progress (counts, states)
void zox_tst_progress(double value) {
    if (!zox_tst_progress_used || value != zox_tst_progress_value) {
        if (zox_tst_progress_used && zox_current_time - zox_tst_progress_time > zox_tst_progress_longest) {
            zox_tst_progress_longest = zox_current_time - zox_tst_progress_time;
        }
        zox_tst_progress_used = 1;
        zox_tst_progress_value = value;
        zox_tst_progress_time = zox_current_time;
    }
}

// Logs and counts a failure when cond is false
#define zox_tst_check(cond, ...) {\
    zox_tst_checks++;\
    if (!(cond)) {\
        zox_tst_fails++;\
        zox_loge("[tst] FAIL: " __VA_ARGS__);\
    }\
}

void zox_tst_add(
    const char* name,
    zox_tst_ready_fun ready,
    zox_tst_check_fun check,
    double timeout)
{
    if (zox_tsts_count >= zox_tsts_max) {
        zox_loge("[tst] too many tests, raise zox_tsts_max");
        return;
    }
    zox_tsts[zox_tsts_count++] = (zox_tst) { name, ready, check, timeout, NULL, 0, 0 };
}

// Adds a setup step (before boot) to a test, and whether it runs through the menus
void zox_tst_set_setup(
    const char* name,
    zox_tst_setup_fun setup,
    byte menus)
{
    for (int i = 0; i < zox_tsts_count; i++) {
        if (strcmp(zox_tsts[i].name, name) == 0) {
            zox_tsts[i].setup = setup;
            zox_tsts[i].menus = menus;
            return;
        }
    }
    zox_loge("[tst] can't set setup, no test [%s]", name);
}

// the test passes when the app closes itself, its ready function triggers the close
//  - main calls zox_tst_main_ended after the main loop, then returns normally, so a crash
//      or hang while closing fails it (exit code, or bsh/tst.sh's timeout)
void zox_tst_set_exits(const char* name) {
    for (int i = 0; i < zox_tsts_count; i++) {
        if (strcmp(zox_tsts[i].name, name) == 0) {
            zox_tsts[i].exits = 1;
            return;
        }
    }
    zox_loge("[tst] can't set exits, no test [%s]", name);
}

void zox_tst_end(const char* reason) {
    if (zox_tst_progress_used) {
        zox_log("[tst] longest wait without progress [%.1fs] of [%.0fs] allowed",
            zox_tst_progress_longest,
            zox_tst_stall_seconds);
    }
    if (zox_tst_fails) {
        zox_loge("[tst] [%s] FAILED [%i/%i] %s (errors logged [%u])",
            zox_tst_active->name,
            zox_tst_fails,
            zox_tst_checks,
            reason,
            zox_log_errors_count - zox_tst_errors_start);
        zox_exit_code = 1;
    } else {
        zox_log("[tst] [%s] PASSED [%i] checks",
            zox_tst_active->name,
            zox_tst_checks);
    }
    zox_tst_active = NULL;
    // exit now, the result is logged, and app teardown has its own (unrelated) crashes
    fflush(stdout);
    fflush(stderr);
    _exit(zox_exit_code);
}

void zox_tst_start();

// called by main once the main loop has ended
void zox_tst_main_ended() {
    if (!zox_tst_active) {
        return;
    }
    if (!zox_tst_active->exits) {
        zox_loge("[tst] [%s] FAILED, the app closed before the test finished", zox_tst_active->name);
        zox_exit_code = 1;
    } else if (zox_tst_fails) {
        zox_loge("[tst] [%s] FAILED [%i/%i] before closing", zox_tst_active->name, zox_tst_fails, zox_tst_checks);
        zox_exit_code = 1;
    } else {
        zox_log("[tst] [%s] PASSED [%i] checks, the main loop ended, closing", zox_tst_active->name, zox_tst_checks);
    }
    fflush(stdout);
    fflush(stderr);
}

void zox_tst_update(ecs* world) {
    if (zox_tst_name) {
        zox_tst_start();
        zox_tst_name = NULL;
    }
    if (!zox_tst_active) {
        return;
    }
    // NOTE: zox_log_errors_count isn't atomic, it's a spam detector, not an exact count
    const uint errors = zox_log_errors_count - zox_tst_errors_start;
    if (errors > zox_tst_errors_max) {
        zox_tst_fails++;
        zox_tst_end("too many errors logged, failing fast (raise with --tst-errors <n>)");
        return;
    }
    if (zox_tst_active->ready(world) && !zox_tst_active->exits) {
        zox_tst_active->check(world);
        zox_tst_end("");
        return;
    }
    if (zox_tst_progress_used && zox_current_time - zox_tst_progress_time > zox_tst_stall_seconds) {
        zox_tst_fails++;
        zox_loge("[tst] stalled, no progress for [%.0fs] waiting for: %s", zox_tst_stall_seconds, zox_tst_waiting);
        zox_tst_end("stalled, failing fast");
        return;
    }
    if (zox_current_time - zox_tst_heartbeat_time > zox_tst_heartbeat_seconds) {
        zox_tst_heartbeat_time = zox_current_time;
        zox_log("[tst] still waiting [%.0fs] for: %s (gpu buffers [%i] freed [%i] textures [%i])", zox_current_time - zox_tst_started_time, zox_tst_waiting, zox_gpu_buffers_live, zox_gpu_buffers_freed, zox_gpu_textures_live);
    }
    if (zox_current_time > zox_tst_active->timeout * zox_tst_timescale) {
        zox_tst_fails++;
        zox_loge("[tst] timed out waiting for: %s", zox_tst_waiting);
        zox_tst_end("timed out before ready");
    }
}

// Runs on the first frame, after all modules have added their tests
void zox_tst_start() {
    for (int i = 0; i < zox_tsts_count; i++) {
        if (strcmp(zox_tsts[i].name, zox_tst_name) == 0) {
            zox_tst_active = &zox_tsts[i];
            zox_tst_errors_start = zox_log_errors_count;
            zox_tst_started_time = zox_current_time;
            zox_tst_heartbeat_time = zox_current_time;
            zox_log("[tst] running [%s], timeout [%.0fs]",
                zox_tst_active->name,
                zox_tst_active->timeout * zox_tst_timescale);
            return;
        }
    }
    if (strcmp(zox_tst_name, "list") != 0) {
        zox_loge("[tst] unknown test [%s]", zox_tst_name);
        zox_exit_code = 1;
    }
    // NOTE: bsh/tst.sh parses "[tst] test: <name>"
    for (int i = 0; i < zox_tsts_count; i++) {
        zox_log("[tst] test: %s", zox_tsts[i].name);
    }
    fflush(stdout);
    _exit(zox_exit_code);
}

// Runs the active test and the frame profiler, at the end of each frame
//  - immediate: a test's checks see their changes straight away
//  - disabled unless --tst or --profile, so games have no merge for it
void zox_tst_system(iter* it) {
    zox_sys_world2();
    zox_tst_update(world);
    zox_profile_update(world);
} zoxd_system(zox_tst_system);

void process_arguments_tst(ecs* world, char* args[], int count) {
    for (int i = 1; i < count; i++) {
        if (strcmp(args[i], "--profile-ms") == 0 && i + 1 < count) {
            zox_profile_spike_ms = atof(args[i + 1]);
        }
        if (strcmp(args[i], "--profile") == 0) {
            zox_profile_spikes = 1;
            zox_log("+ profiling, frames over [%.0fms] are logged with their slowest systems", zox_profile_spike_ms);
        }
    }
    for (int i = 1; i < count - 1; i++) {
        if (strcmp(args[i], "--tst") == 0) {
            zox_tst_name = args[i + 1];
            // tests are registered by now (module imports), setup runs before boot
            for (int j = 0; j < zox_tsts_count; j++) {
                if (strcmp(zox_tsts[j].name, zox_tst_name) == 0) {
                    zox_tst_menus = zox_tsts[j].menus;
                    if (zox_tsts[j].setup) {
                        zox_tsts[j].setup();
                    }
                }
            }
            // flush every log line, so a crash doesn't lose the end of the log
            setvbuf(stdout, NULL, _IOLBF, 0);
            setvbuf(stderr, NULL, _IONBF, 0);
        } else if (strcmp(args[i], "--tst-errors") == 0 && i + 1 < count) {
            zox_tst_errors_max = (uint) atoi(args[i + 1]);
        } else if (strcmp(args[i], "--tst-timescale") == 0 && i + 1 < count) {
            zox_tst_timescale = atof(args[i + 1]);
        }
    }
    if (zox_tst_name || zox_profile_spikes) {
        ecs_enable(world, ecs_id(zox_tst_system), true);
    }
}

// module tests below core (the "_" module), registered here as they can't see the harness
#include "../../_/octrees/tst/_.c"
#include "../../_/collections/tst/_.c"
#include "../../_/flecs/tst/_.c"
#include "../../_/maths/tst/_.c"


void initialize_tst(ecs* world) {
    zox_tsts_octrees();
    zox_tsts_hashmaps();
    zox_tsts_maths();
    add_hook_terminal_command(process_arguments_tst);
    zox_system_immediate(
        zox_tst_system,
        zoxp_dbg_end,
        0
    );
    ecs_enable(world, ecs_id(zox_tst_system), false);
    zox_system_times_ignore = ecs_id(zox_tst_system);
}
