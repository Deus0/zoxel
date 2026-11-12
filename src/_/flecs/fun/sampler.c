// Sampling profiler: ZOX_SAMPLE=path, the main thread's backtrace every 2ms with its frame, written at exit
//  - for when perf isn't there and ptrace is restricted (ptrace_scope 1), summarise with bsh/sample_report.py
//  - found deleting walking every non fragmenting record (bsh/flecs_nf_root.py), see doc/commands.md
#include <time.h>
#include <execinfo.h>
#include <signal.h>
#include <sys/syscall.h>
#include <unistd.h>
extern long syscall(long number, ...);
#define zox_sample_max 30000
#define zox_sample_depth 32
static void* zox_samples[zox_sample_max][zox_sample_depth];
static int zox_sample_depths[zox_sample_max];
static uint zox_sample_frames[zox_sample_max];
static volatile int zox_sample_count = 0;

static void zox_sample_handler(int sig) {
    (void) sig;
    const int i = zox_sample_count;
    if (i >= zox_sample_max) {
        return;
    }
    zox_sample_depths[i] = backtrace(zox_samples[i], zox_sample_depth);
    zox_sample_frames[i] = ecs_run_count;
    zox_sample_count = i + 1;
}

static void zox_sample_write() {
    const char* path = getenv("ZOX_SAMPLE");
    if (!path) {
        return;
    }
    FILE* f = fopen(path, "w");
    if (!f) {
        return;
    }
    fprintf(f, "base %p\n", (void*) zox_sample_handler);
    for (int i = 0; i < zox_sample_count; i++) {
        fprintf(f, "s %u", zox_sample_frames[i]);
        for (int d = 0; d < zox_sample_depths[i]; d++) {
            fprintf(f, " %p", zox_samples[i][d]);
        }
        fprintf(f, "\n");
    }
    fclose(f);
}

static void zox_sample_start() {
    if (!getenv("ZOX_SAMPLE")) {
        return;
    }
    void* warm[4];
    backtrace(warm, 4);
    struct sigaction sa = { 0 };
    sa.sa_handler = zox_sample_handler;
    sa.sa_flags = SA_RESTART;
    sigaction(SIGPROF, &sa, NULL);
    struct sigevent sev = { 0 };
    sev.sigev_notify = SIGEV_THREAD_ID;
    sev._sigev_un._tid = syscall(SYS_gettid);
    sev.sigev_signo = SIGPROF;
    timer_t timer;
    timer_create(CLOCK_MONOTONIC, &sev, &timer);
    struct itimerspec its = { { 0, 2000000 }, { 0, 2000000 } };
    timer_settime(timer, 0, &its, NULL);
    atexit(zox_sample_write);
}

