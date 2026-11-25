// Frame times for the perf tests (perf_idle, perf_stream): every frame's time kept, then a summary anyone can read
//  - frames and seconds, fps (average, and the 1% low: the slowest 1% of frames' average as fps), frame times
//    (fastest, median, average, 99th percentile, slowest), frames missing 120 fps (the target), 90 and 72 (vr), 60, 30
//  - the test's own time (zox_tst_frames_exclude: a settle check walking every chunk) is left out, it isn't the game's
//  - the label says what was measured: backend, far distance, cpu or gpu meshing
#define zox_tst_frames_max 100000

typedef struct {
    double* times;
    int count;
    double last;
    double excluded;    // the test's own time this frame (zox_tst_frames_exclude), not the game's
} zox_tst_frames;

static inline void zox_tst_frames_begin(zox_tst_frames* frames) {
    if (!frames->times) {
        frames->times = malloc(sizeof(double) * zox_tst_frames_max);
    }
    frames->count = 0;
    frames->last = get_time_ms();
    frames->excluded = 0;
}

// time the test spent this frame (i.e. walking every chunk to see if streaming settled, ~13ms a frame at far 32): left
//  out of the frame's time, it isn't the game's
static inline void zox_tst_frames_exclude(zox_tst_frames* frames, double ms) {
    frames->excluded += ms;
    zox_tst_profile_exclude(ms);
}

// a frame ended now
static inline void zox_tst_frames_tick(zox_tst_frames* frames) {
    const double now = get_time_ms();
    if (frames->times && frames->count < zox_tst_frames_max) {
        const double ms = now - frames->last - frames->excluded;
        frames->times[frames->count++] = ms > 0 ? ms : 0;
    }
    frames->last = now;
    frames->excluded = 0;
}

static int zox_tst_frames_compare(const void* a, const void* b) {
    const double x = *(const double*) a, y = *(const double*) b;
    return x < y ? -1 : x > y;
}

// what's being measured, i.e. "[vulkan] far [32] vertical [3] meshing [gpu] build [release]"
static inline void zox_tst_frames_label(char* label, int size) {
#ifdef zox_debug
    const char* build = "debug";
#else
    const char* build = "release";
#endif
    snprintf(label, size, "[%s] far [%i] vertical [%i] meshing [%s] build [%s]",
        render_backend == zox_render_backend_vulkan ? "vulkan" : "opengl", terrain_lod_far, render_distance_y,
        zox_chunks3_gpu_meshing ? "gpu" : "cpu", build);
}

// a perf test's scale, its results' first line: a tiny world (tst.sh's default in a full run) is a smoke run, its
//  numbers aren't the game's (its far lods, most of the terrain, are missing); alone a perf test runs at far 8
static inline void zox_tst_perf_scale(const char* name) {
    char label[128];
    zox_tst_frames_label(label, sizeof(label));
    zox_log("[tst] [metric] %s scale %s%s", name, label,
        terrain_lod_far < 8 ? ": small, a smoke run, not the game's scale (run it alone, at far 8)" : "");
}

static inline void zox_tst_frames_log(zox_tst_frames* frames, const char* name) {
    const int count = frames->count;
    char label[128];
    zox_tst_frames_label(label, sizeof(label));
    if (!count) {
        zox_log("[tst] [metric] %s %s: no frames", name, label);
        return;
    }
    double* sorted = malloc(sizeof(double) * count);
    memcpy(sorted, frames->times, sizeof(double) * count);
    qsort(sorted, count, sizeof(double), zox_tst_frames_compare);
    double total = 0;
    int over_120 = 0, over_90 = 0, over_72 = 0, over_60 = 0, over_30 = 0;
    for (int i = 0; i < count; i++) {
        total += sorted[i];
        over_120 += sorted[i] > 1000.0 / 120;
        over_90 += sorted[i] > 1000.0 / 90;
        over_72 += sorted[i] > 1000.0 / 72;
        over_60 += sorted[i] > 1000.0 / 60;
        over_30 += sorted[i] > 1000.0 / 30;
    }
    // the slowest 1% (at least one frame), their average as fps
    const int low_count = count / 100 > 0 ? count / 100 : 1;
    double low_total = 0;
    for (int i = count - low_count; i < count; i++) {
        low_total += sorted[i];
    }
    const double average = total / count;
    zox_log("[tst] [metric] %s %s: frames [%i] in [%.2fs]", name, label, count, total / 1000.0);
    zox_log("[tst] [metric] %s %s: fps average [%.1f] 1%% low [%.1f]", name, label, 1000.0 / average,
        1000.0 / (low_total / low_count));
    zox_log("[tst] [metric] %s %s: frame ms fastest [%.2f] median [%.2f] average [%.2f] 99th [%.2f] slowest [%.2f]",
        name, label, sorted[0], sorted[count / 2], average, sorted[count - 1 - count / 100], sorted[count - 1]);
    zox_log("[tst] [metric] %s %s: frames over 8.3ms (under 120fps, the target) [%i], 11.1ms (under 90fps, vr) [%i], 13.9ms (under 72fps, vr) [%i]",
        name, label, over_120, over_90, over_72);
    zox_log("[tst] [metric] %s %s: frames over 16.7ms (under 60fps) [%i], over 33.3ms (under 30fps) [%i]",
        name, label, over_60, over_30);
    free(sorted);
}

static inline void zox_tst_frames_dispose(zox_tst_frames* frames) {
    free(frames->times);
    frames->times = NULL;
    frames->count = 0;
}
