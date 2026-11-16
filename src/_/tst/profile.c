// A frame profiler for tests: finds what makes a frame slow (lag spikes)
//  - zox_tst_profile_frame once a frame: the frame's time, and every system's time that frame
//  - multi threaded systems' times are cpu time summed over workers, divided by the workers for real
//      time (an estimate, workers can be uneven), the systems' real time is what spikes a frame
//  - tests render in software, so whole frames are slow, the systems' real time is the measure
//  - remembers the slowest frames and each system's slowest frame
//  - zox_tst_profile_log: the slowest frames with their slowest systems, the systems with the biggest frames
//  - also while playing: --profile logs every frame over zox_profile_spike_ms with its slowest systems
//      ("render" systems include waiting on the gpu)
#define zox_tst_profile_max_systems 2048
byte zox_profile_spikes = 0;
double zox_profile_spike_ms = 25;
#define zox_tst_profile_worst_frames 5
#define zox_tst_profile_top 3

typedef struct {
    entity system;
    double total;       // seconds, last seen
    double frame_ms;    // this frame
    double worst_ms;    // its slowest frame
    int worst_frame;
    double sum_ms;
    byte multi_threaded;
} ZoxTstProfileSystem;

typedef struct {
    double ms;
    int frame;
    double wall_ms;     // the whole frame, rendering too
    entity top[zox_tst_profile_top];
    double top_ms[zox_tst_profile_top];
} ZoxTstProfileFrame;

ZoxTstProfileSystem zox_tst_profile_systems[zox_tst_profile_max_systems];
int zox_tst_profile_systems_count = 0;
ZoxTstProfileFrame zox_tst_profile_frames[zox_tst_profile_worst_frames];
int zox_tst_profile_frame_index = 0;
double zox_tst_profile_last_ms = 0;
double zox_tst_profile_ecs_worst_ms = 0;    // the most time systems took in one frame
uintptr_t* zox_tst_profile_tables_before = NULL;   // last frame's tables (--profile)
int zox_tst_profile_tables_count = 0;
ZoxWorldTimes zox_tst_profile_world_last;
// observers (zox_observer_timings): their totals last frame, this frame, their slowest frame
double zox_tst_profile_observer_last[zox_observer_timings_max];
double zox_tst_profile_observer_frame[zox_observer_timings_max];
double zox_tst_profile_observer_worst[zox_observer_timings_max];

static void zox_tst_profile_system(ecs* world, entity system, double seconds, byte multi_threaded, void* ctx) {
    int* hint = (int*) ctx;
    ZoxTstProfileSystem* found = NULL;
    // systems come in the same order each frame, so the next one is likely the hint
    if (*hint < zox_tst_profile_systems_count && zox_tst_profile_systems[*hint].system == system) {
        found = &zox_tst_profile_systems[*hint];
    } else {
        for (int i = 0; i < zox_tst_profile_systems_count; i++) {
            if (zox_tst_profile_systems[i].system == system) {
                found = &zox_tst_profile_systems[i];
                *hint = i;
                break;
            }
        }
    }
    if (!found) {
        if (zox_tst_profile_systems_count >= zox_tst_profile_max_systems) {
            return;
        }
        *hint = zox_tst_profile_systems_count;
        found = &zox_tst_profile_systems[zox_tst_profile_systems_count++];
        *found = (ZoxTstProfileSystem) { .system = system, .total = seconds, .worst_frame = -1, .multi_threaded = multi_threaded };
        (*hint)++;
        return;
    }
    found->frame_ms = (seconds - found->total) * 1000.0 / (multi_threaded ? zox_worker_count(world) : 1);
    found->total = seconds;
    found->sum_ms += found->frame_ms;
    if (found->frame_ms > found->worst_ms) {
        found->worst_ms = found->frame_ms;
        found->worst_frame = zox_tst_profile_frame_index;
    }
    (*hint)++;
}

// when this frame's pipeline started (zox_tst_frame_begin_system, zoxp_dbg_begin)
double zox_tst_profile_frame_start = 0;

void zox_tst_profile_frame(ecs* world) {
    if (!zox_tst_profile_frame_index && !zox_tst_profile_systems_count) {
        zox_measure_system_times(world);
    }
    const double now = get_time_ms();
    const double frame_ms = zox_tst_profile_last_ms ? now - zox_tst_profile_last_ms : 0;
    zox_tst_profile_last_ms = now;
    int hint = 0;
    zox_each_system_time(world, zox_tst_profile_system, &hint);
    const ZoxWorldTimes world_times = zox_world_times(world);
    const double merge_ms = (world_times.merge - zox_tst_profile_world_last.merge) * 1000.0;
    const double emit_ms = (world_times.emit - zox_tst_profile_world_last.emit) * 1000.0;
    // the pipeline so far (zoxp_dbg_begin to here), flecs' own frame total is only set after the frame
    const double ecs_frame_ms = zox_tst_profile_frame_start ? now - zox_tst_profile_frame_start : 0;
    const ZoxWorldTimes last = zox_tst_profile_world_last;
    zox_tst_profile_world_last = world_times;
    int top_observers[zox_tst_profile_top] = { -1, -1, -1 };
    for (int i = 0; i < zox_observer_timings_count; i++) {
        const double total = zox_observer_timings[i].total_ms;
        const double ms = total - zox_tst_profile_observer_last[i];
        zox_tst_profile_observer_last[i] = total;
        zox_tst_profile_observer_frame[i] = ms;
        if (zox_tst_profile_frame_index >= 2 && ms > zox_tst_profile_observer_worst[i]) {
            zox_tst_profile_observer_worst[i] = ms;
        }
        for (int t = 0; t < zox_tst_profile_top; t++) {
            if (top_observers[t] < 0 || ms > zox_tst_profile_observer_frame[top_observers[t]]) {
                for (int k = zox_tst_profile_top - 1; k > t; k--) {
                    top_observers[k] = top_observers[k - 1];
                }
                top_observers[t] = i;
                break;
            }
        }
    }
    zox_tst_profile_frame_index++;
    if (zox_tst_profile_frame_index < 3) {
        return; // the first frames only set totals
    }
    // the frame's slowest systems
    ZoxTstProfileFrame frame = { .ms = frame_ms, .wall_ms = frame_ms, .frame = zox_tst_profile_frame_index };
    double ecs_ms = 0;
    for (int i = 0; i < zox_tst_profile_systems_count; i++) {
        const ZoxTstProfileSystem* s = &zox_tst_profile_systems[i];
        ecs_ms += s->frame_ms;
        for (int t = 0; t < zox_tst_profile_top; t++) {
            if (s->frame_ms > frame.top_ms[t]) {
                for (int k = zox_tst_profile_top - 1; k > t; k--) {
                    frame.top[k] = frame.top[k - 1];
                    frame.top_ms[k] = frame.top_ms[k - 1];
                }
                frame.top[t] = s->system;
                frame.top_ms[t] = s->frame_ms;
                break;
            }
        }
    }
    zox_tst_profile_ecs_worst_ms = fmax(zox_tst_profile_ecs_worst_ms, ecs_ms);
    frame.ms = ecs_ms;
    // --profile: a slow frame while playing is logged with its slowest systems
    if (zox_profile_spikes && frame.wall_ms > zox_profile_spike_ms) {
        zox_log("[profile] frame [%i] took [%.1fms], ecs [%.1fms] merges [%.1fms] observers [%.1fms], systems [%.1fms]: %s [%.1fms], %s [%.1fms], %s [%.1fms]",
            frame.frame, frame.wall_ms, ecs_frame_ms, merge_ms, emit_ms, ecs_ms,
            frame.top[0] ? zox_get_name(frame.top[0]) : "-", frame.top_ms[0],
            frame.top[1] ? zox_get_name(frame.top[1]) : "-", frame.top_ms[1],
            frame.top[2] ? zox_get_name(frame.top[2]) : "-", frame.top_ms[2]);
        for (int t = 0; t < zox_tst_profile_top; t++) {
            const int o = top_observers[t];
            if (o >= 0 && zox_tst_profile_observer_frame[o] > 0.5) {
                zox_log("[profile]   observer %s [%.1fms]", zox_observer_timings[o].name, zox_tst_profile_observer_frame[o]);
            }
        }
        // the families of the tables this frame made (each new table notifies every observer)
        if (zox_tst_profile_tables_before) {
            static char families[64][zox_table_family_length];
            static int counts[64];
            int total = 0;
            const int count = zox_new_table_families(world, zox_tst_profile_tables_before, zox_tst_profile_tables_count, families, counts, 64, &total);
            for (int f = 0; f < count && f < 3; f++) {
                const int length = (int) strlen(families[f]);
                zox_log("[profile]   new tables [%i]: %.90s ... %.150s", counts[f], families[f], families[f] + (length > 150 ? length - 150 : 0));
            }
        }
        {
            static char spawned[1024];
            zox_count_spawned_prefabs(world, spawned, sizeof(spawned), 8);
            zox_log("[profile]   spawned most: %.400s", spawned);
        }
        {
            static char top_ids[1024];
            zox_count_ids_top(world, top_ids, sizeof(top_ids), 8);
            zox_log("[profile]   moved most: %.400s", top_ids);
        }
        zox_log("[profile]   commands: adds [%li] removes [%li] deletes [%li] sets [%li], observers ran [%li], tables made [%li]",
            (long) (world_times.adds - last.adds), (long) (world_times.removes - last.removes), (long) (world_times.deletes - last.deletes),
            (long) (world_times.sets - last.sets), (long) (world_times.observers - last.observers), (long) (world_times.tables - last.tables));
    }
    if (!(zox_profile_spikes && frame.wall_ms > zox_profile_spike_ms)) {
        memset(zox_id_counts, 0, sizeof(zox_id_counts));
    }
    // the tables now, the next frame's new ones are what it made (only with --profile, it's a scan)
    if (zox_profile_spikes) {
        if (zox_tst_profile_tables_before) {
            free(zox_tst_profile_tables_before);
        }
        zox_tst_profile_tables_before = zox_tables_snapshot(world, &zox_tst_profile_tables_count);
    }
    // keep the slowest frames
    for (int w = 0; w < zox_tst_profile_worst_frames; w++) {
        if (frame.ms > zox_tst_profile_frames[w].ms) {
            for (int k = zox_tst_profile_worst_frames - 1; k > w; k--) {
                zox_tst_profile_frames[k] = zox_tst_profile_frames[k - 1];
            }
            zox_tst_profile_frames[w] = frame;
            break;
        }
    }
}

static int zox_tst_profile_compare(const void* a, const void* b) {
    const double x = ((const ZoxTstProfileSystem*) a)->worst_ms;
    const double y = ((const ZoxTstProfileSystem*) b)->worst_ms;
    return x < y ? 1 : x > y ? -1 : 0;
}

void zox_tst_profile_log(ecs* world, const char* name) {
    zox_log("[tst] [metric] %s frames [%i], workers [%i], slowest frame of systems [%.1fms] (real time, est)", name,
        zox_tst_profile_frame_index, zox_worker_count(world), zox_tst_profile_ecs_worst_ms);
    for (int w = 0; w < zox_tst_profile_worst_frames && zox_tst_profile_frames[w].ms > 0; w++) {
        const ZoxTstProfileFrame* f = &zox_tst_profile_frames[w];
        zox_log("[tst] [metric] %s slow frame [%i] systems [%.1fms]: %s [%.1fms], %s [%.1fms], %s [%.1fms]", name, f->frame, f->ms,
            f->top[0] ? zox_get_name(f->top[0]) : "-", f->top_ms[0],
            f->top[1] ? zox_get_name(f->top[1]) : "-", f->top_ms[1],
            f->top[2] ? zox_get_name(f->top[2]) : "-", f->top_ms[2]);
    }
    // what moved entities between tables most (adds and removes, each a table move and observer checks)
    {
        static char top_ids[2048];
        zox_count_ids_totals(world, top_ids, sizeof(top_ids), 14);
        zox_log("[tst] [metric] %s moved most: %.400s", name, top_ids);
    }
    // the observers with the slowest frames
    for (int n = 0; n < 8; n++) {
        int best = -1;
        for (int i = 0; i < zox_observer_timings_count; i++) {
            if (zox_tst_profile_observer_worst[i] > 0 && (best < 0 || zox_tst_profile_observer_worst[i] > zox_tst_profile_observer_worst[best])) {
                best = i;
            }
        }
        if (best < 0) {
            break;
        }
        zox_log("[tst] [metric] %s observer [%s] slowest frame [%.1fms] total [%.1fms] calls [%u]", name,
            zox_observer_timings[best].name, zox_tst_profile_observer_worst[best], zox_observer_timings[best].total_ms, zox_observer_timings[best].calls);
        zox_tst_profile_observer_worst[best] = -zox_tst_profile_observer_worst[best];   // listed
    }
    qsort(zox_tst_profile_systems, zox_tst_profile_systems_count, sizeof(ZoxTstProfileSystem), zox_tst_profile_compare);
    for (int i = 0; i < 12 && i < zox_tst_profile_systems_count; i++) {
        const ZoxTstProfileSystem* s = &zox_tst_profile_systems[i];
        zox_log("[tst] [metric] %s system [%s]%s slowest frame [%.1fms] (frame %i) average [%.2fms]", name,
            zox_get_name(s->system), s->multi_threaded ? " (mt)" : "", s->worst_ms, s->worst_frame, s->sum_ms / fmax(1, zox_tst_profile_frame_index - 2));
    }
}

// --profile, every frame
void zox_profile_update(ecs* world) {
    if (zox_profile_spikes) {
        zox_tst_profile_frame(world);
    }
}
