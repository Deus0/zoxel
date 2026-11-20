// A frame profiler for tests: finds what makes a frame slow (lag spikes)
//  - zox_tst_profile_frame once a frame: the frame's time, and every system's time that frame
//  - a system's time is its longest run on one thread this frame (zox_sys_on_begin to zox_sys_on_end: its biggest
//      piece of work, what holds up the systems run with it); a system without one, flecs' time (multi threaded: its
//      cpu time over the workers, an estimate)
//  - the frame's systems time is the union of their spans (first thread's start to last thread's end), real time:
//      workers run a phase's systems back to back, their spans overlap
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
    int span;           // its zox_system_spans index, -1 none
    double unspanned_ms;    // flecs' time on frames it had no span (no zox_sys_on_begin / zox_sys_on_end)
} ZoxTstProfileSystem;

typedef struct {
    double ms;
    int frame;
    double wall_ms;     // the whole frame, rendering too
    double gap_ms;      // its largest gap between systems, after gap_after, before gap_before
    entity gap_after;
    entity gap_before;
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

// every frame's accounting summed (zox_tst_profile_log averages it): the whole frame, its systems (spans' union), flecs'
//  merges (deferred changes applied) and observers, the rest (rendering waits, presenting, the loop)
double zox_tst_profile_sum_wall = 0, zox_tst_profile_sum_systems = 0, zox_tst_profile_sum_merges = 0, zox_tst_profile_sum_observers = 0;
double zox_tst_profile_sum_pipeline = 0;    // the pipeline's start (zoxp_dbg_begin) to the profiler's call
double zox_tst_profile_sum_progress = 0, zox_tst_profile_sum_rematch = 0;   // flecs' whole frame, its query rematching
int zox_tst_profile_sum_frames = 0;

// the test's own time this frame (a settle check walking every chunk), left out of the frame's time
double zox_tst_profile_excluded = 0;

static inline void zox_tst_profile_exclude(double ms) {
    zox_tst_profile_excluded += ms;
}

// this frame's system spans, their union is the systems' real time in it
#define zox_tst_profile_spans_max 2048
ulong zox_tst_profile_spans[zox_tst_profile_spans_max][3];   // begin, end, its system
int zox_tst_profile_spans_count = 0;

static inline void zox_tst_profile_span_add(ulong begin, ulong end, entity system) {
    if (zox_tst_profile_spans_count < zox_tst_profile_spans_max) {
        zox_tst_profile_spans[zox_tst_profile_spans_count][0] = begin;
        zox_tst_profile_spans[zox_tst_profile_spans_count][1] = end;
        zox_tst_profile_spans[zox_tst_profile_spans_count][2] = system;
        zox_tst_profile_spans_count++;
    }
}

static int zox_tst_profile_span_compare(const void* a, const void* b) {
    const ulong x = ((const ulong*) a)[0], y = ((const ulong*) b)[0];
    return x < y ? -1 : x > y;
}

// the frame's largest gap between systems' spans: no system running (flecs between them), after one, before the next
double zox_tst_profile_gap_ms = 0;
entity zox_tst_profile_gap_after = 0, zox_tst_profile_gap_before = 0;

// the union of this frame's spans in ms (overlaps once), its largest gap, then cleared
static inline double zox_tst_profile_spans_union() {
    qsort(zox_tst_profile_spans, zox_tst_profile_spans_count, sizeof(zox_tst_profile_spans[0]), zox_tst_profile_span_compare);
    ulong total = 0, begin = 0, end = 0;
    entity end_system = 0;
    zox_tst_profile_gap_ms = 0;
    for (int i = 0; i < zox_tst_profile_spans_count; i++) {
        if (i == 0 || zox_tst_profile_spans[i][0] > end) {
            if (i > 0 && (zox_tst_profile_spans[i][0] - end) / 1000.0 > zox_tst_profile_gap_ms) {
                zox_tst_profile_gap_ms = (zox_tst_profile_spans[i][0] - end) / 1000.0;
                zox_tst_profile_gap_after = end_system;
                zox_tst_profile_gap_before = (entity) zox_tst_profile_spans[i][2];
            }
            total += end - begin;
            begin = zox_tst_profile_spans[i][0];
            end = zox_tst_profile_spans[i][1];
            end_system = (entity) zox_tst_profile_spans[i][2];
        } else if (zox_tst_profile_spans[i][1] > end) {
            end = zox_tst_profile_spans[i][1];
            end_system = (entity) zox_tst_profile_spans[i][2];
        }
    }
    total += end - begin;
    zox_tst_profile_spans_count = 0;
    return total / 1000.0;
}

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
        *found = (ZoxTstProfileSystem) { .system = system, .total = seconds, .worst_frame = -1, .multi_threaded = multi_threaded,
            .span = zox_system_index(system) };
        (*hint)++;
        return;
    }
    // its span this frame (real time), else flecs' time
    zox_system_span* span = found->span >= 0 ? &zox_system_spans[found->span] : NULL;
    // (begin 0: not reset yet, the first frame)
    if (span && span->end && span->begin && span->begin < span->end) {
        found->frame_ms = span->slice / 1000.0;
        zox_tst_profile_span_add(span->begin, span->end, system);
    } else if (span && span->end) {
        found->frame_ms = 0;    // it ran, too quick to see
    } else {
        found->frame_ms = (seconds - found->total) * 1000.0 / (multi_threaded ? zox_worker_count(world) : 1);
        found->unspanned_ms += (seconds - found->total) * 1000.0;
    }
    if (span) {
        span->begin = (ulong) -1;
        span->end = 0;
        span->slice = 0;
    }
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
    const double excluded_ms = zox_tst_profile_excluded;
    const double frame_ms = zox_tst_profile_last_ms ? now - zox_tst_profile_last_ms - excluded_ms : 0;
    zox_tst_profile_excluded = 0;
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
    // real time: the spans' union (overlapping systems once), plus systems without spans
    const double spans_ms = zox_tst_profile_spans_union();
    double unspanned_ms = 0;
    for (int i = 0; i < zox_tst_profile_systems_count; i++) {
        const ZoxTstProfileSystem* s = &zox_tst_profile_systems[i];
        unspanned_ms += s->span < 0 ? s->frame_ms : 0;
    }
    ecs_ms = spans_ms + unspanned_ms;
    if (frame_ms > 0) {
        zox_tst_profile_sum_wall += frame_ms;
        zox_tst_profile_sum_systems += ecs_ms;
        zox_tst_profile_sum_merges += merge_ms;
        zox_tst_profile_sum_observers += emit_ms;
        zox_tst_profile_sum_pipeline += ecs_frame_ms;
        // the test's own time ran inside flecs' frame too
        zox_tst_profile_sum_progress += (world_times.frame - last.frame) * 1000.0 - excluded_ms;
        zox_tst_profile_sum_rematch += (world_times.rematch - last.rematch) * 1000.0;
        zox_tst_profile_sum_frames++;
    }
    zox_tst_profile_ecs_worst_ms = fmax(zox_tst_profile_ecs_worst_ms, ecs_ms);
    frame.ms = ecs_ms;
    frame.gap_ms = zox_tst_profile_gap_ms;
    frame.gap_after = zox_tst_profile_gap_after;
    frame.gap_before = zox_tst_profile_gap_before;
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
    if (zox_tst_profile_sum_frames) {
        const double n = zox_tst_profile_sum_frames;
        const double wall = zox_tst_profile_sum_wall / n, systems = zox_tst_profile_sum_systems / n;
        const double merges = zox_tst_profile_sum_merges / n, observers = zox_tst_profile_sum_observers / n;
        const double pipeline = zox_tst_profile_sum_pipeline / n;
        zox_log("[tst] [metric] %s an average frame [%.2fms]: systems [%.2fms] merges [%.2fms] observers [%.2fms] the rest [%.2fms] (rendering waits, presenting, the loop)",
            name, wall, systems, merges, observers, wall - systems - merges - observers);
        const double progress = zox_tst_profile_sum_progress / n, rematch = zox_tst_profile_sum_rematch / n;
        zox_log("[tst] [metric] %s an average frame: flecs' progress [%.2fms] (its query rematching [%.2fms]), outside flecs [%.2fms]",
            name, progress, rematch, wall - progress);
    }
    for (int w = 0; w < zox_tst_profile_worst_frames && zox_tst_profile_frames[w].ms > 0; w++) {
        const ZoxTstProfileFrame* f = &zox_tst_profile_frames[w];
        zox_log("[tst] [metric] %s slow frame [%i] systems [%.1fms]: %s [%.1fms], %s [%.1fms], %s [%.1fms]", name, f->frame, f->ms,
            f->top[0] ? zox_get_name(f->top[0]) : "-", f->top_ms[0],
            f->top[1] ? zox_get_name(f->top[1]) : "-", f->top_ms[1],
            f->top[2] ? zox_get_name(f->top[2]) : "-", f->top_ms[2]);
        zox_log("[tst] [metric] %s slow frame [%i] took [%.1fms], its largest gap between systems [%.1fms]: after %s, before %s",
            name, f->frame, f->wall_ms, f->gap_ms, f->gap_after ? zox_get_name(f->gap_after) : "-",
            f->gap_before ? zox_get_name(f->gap_before) : "-");
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
    // systems with no spans (no zox_sys_on_begin / end): their time is only flecs' estimate, gaps in the timeline
    for (int n = 0; n < 6; n++) {
        int best = -1;
        for (int i = 0; i < zox_tst_profile_systems_count; i++) {
            if (zox_tst_profile_systems[i].unspanned_ms > 0.5 && (best < 0 || zox_tst_profile_systems[i].unspanned_ms > zox_tst_profile_systems[best].unspanned_ms)) {
                best = i;
            }
        }
        if (best < 0) {
            break;
        }
        zox_log("[tst] [metric] %s system [%s] has no span: flecs' time [%.1fms] over [%i] frames (cpu time, all threads)", name,
            zox_get_name(zox_tst_profile_systems[best].system), zox_tst_profile_systems[best].unspanned_ms, zox_tst_profile_frame_index);
        zox_tst_profile_systems[best].unspanned_ms = -zox_tst_profile_systems[best].unspanned_ms;
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
