// tst: pipeline_syncs, logs the pipeline as it runs in game: its systems in order, by phase, and its merges
//  - a merge (sync point) is where flecs waits for every worker and applies the deferred commands
//  - flecs merges when execution switches between multi threaded and main thread (or immediate) systems,
//    and when a system reads what another wrote through commands, not between phases
//  - a main thread system between multi threaded ones adds 2 merges and leaves the other cores waiting, those
//    are listed (not barriers, zox_phase_barrier): keep main thread systems together (zoxp_spawn), make the
//    rest multi threaded, see doc/thread_safety.md
//  - checks octree writes finish before octree reads (zoxp_octree_sync's merge)
//  - how to read a pipeline's sync points: ecs_pipeline_stats_get, its systems with a 0 at each merge
//  - run: bash bsh/tst.sh pipeline_syncs

#define zox_tst_pipeline_merges_max 12

static const char* zox_tst_pipeline_phase_name(ecs* world, entity system) {
    const entity phase = ecs_get_target(world, system, EcsDependsOn, 0);
    return phase ? ecs_get_name(world, phase) : "?";
}

static inline byte zox_tst_pipeline_is_mt(ecs* world, entity system) {
    const ecs_system_t* s = ecs_system_get(world, system);
    return s && s->multi_threaded && !s->immediate;
}

void zox_tst_pipeline_syncs_check(ecs* world) {
    ecs_pipeline_stats_t stats = { 0 };
    const byte got = ecs_pipeline_stats_get(world, ecs_get_pipeline(world), &stats);
    zox_tst_check(got, "no pipeline stats");
    if (!got) {
        return;
    }
    const entity* systems = ecs_vec_first_t(&stats.systems, entity);
    const int count = ecs_vec_count(&stats.systems);
    int merges = 0;
    int system_count = 0;
    int sandwiched = 0;
    // the pipeline, one line per phase: systems in order, [1] main thread, [!] immediate, | a merge
    char line[512];
    int length = 0;
    const char* phase_name = NULL;
    for (int i = 0; i < count; i++) {
        const entity system = systems[i];
        if (!system) {
            merges++;
            length += snprintf(line + length, sizeof(line) - length, " |");
            continue;
        }
        system_count++;
        const char* phase = zox_tst_pipeline_phase_name(world, system);
        if (!phase_name || strcmp(phase, phase_name)) {
            if (phase_name) {
                zox_log("[pipeline] %s:%s", phase_name, line);
            }
            phase_name = phase;
            length = 0;
            line[0] = 0;
        }
        const ecs_system_t* s = ecs_system_get(world, system);
        const char* tag = !s ? "?" : (s->immediate ? "[!]" : (s->multi_threaded ? "" : "[1]"));
        // wrap long phases, logs clamp long lines
        if (length > 300) {
            zox_log("[pipeline] %s:%s", phase_name, line);
            length = 0;
            line[0] = 0;
        }
        length += snprintf(line + length, sizeof(line) - length, " %s%s", tag, ecs_get_name(world, system));
        // a main thread system between multi threaded ones: 2 merges for one system
        if (s && !s->multi_threaded && !s->immediate) {
            entity before = 0, after = 0;
            for (int j = i - 1; j >= 0; j--) if (systems[j]) { before = systems[j]; break; }
            for (int j = i + 1; j < count; j++) if (systems[j]) { after = systems[j]; break; }
            const char* name = ecs_get_name(world, system);
            const byte is_barrier = name && (strstr(name, "_write") || strstr(name, "_wait"));
            if (!is_barrier && before && after &&
                zox_tst_pipeline_is_mt(world, before) && zox_tst_pipeline_is_mt(world, after)) {
                sandwiched++;
                zox_log("[pipeline] main thread between multi threaded (2 merges): [%s] in [%s]",
                    ecs_get_name(world, system), phase);
            }
        }
    }
    if (phase_name) {
        zox_log("[pipeline] %s:%s", phase_name, line);
    }
    zox_log("[tst] [metric] pipeline systems [%i] merges [%i] main thread between multi threaded [%i]",
        system_count, merges, sandwiched);
    // guards, raise with a reason: 12 merges (2026-11-12), one per op from spawn to the end of the frame, and
    //  zox_tst_system (immediate, only on with --tst or --profile, games have 11)
    zox_tst_check(merges <= zox_tst_pipeline_merges_max, "[%i] merges, over [%i]: a main thread system in a multi \
threaded phase? (listed above)", merges, zox_tst_pipeline_merges_max);
    zox_tst_check(!sandwiched, "[%i] main thread systems between multi threaded ones", sandwiched);
    // octree writes all finish before reads: a merge between the last write and the first read
    int last_write = -1, first_read = -1;
    for (int i = 0; i < count; i++) {
        if (!systems[i]) {
            continue;
        }
        const entity phase = ecs_get_target(world, systems[i], EcsDependsOn, 0);
        if (phase == zoxp_octree_write) {
            last_write = i;
        } else if (phase == zoxp_octree_read && first_read < 0) {
            first_read = i;
        }
    }
    zox_tst_check(last_write >= 0 && first_read >= 0, "no octree write [%i] or read [%i] systems running",
        last_write, first_read);
    if (last_write >= 0 && first_read > last_write) {
        byte merged = 0;
        for (int i = last_write + 1; i < first_read; i++) {
            merged |= !systems[i];
        }
        zox_tst_check(merged, "no merge between octree writes and reads, workers can read an octree being written");
    }
    ecs_pipeline_stats_fini(&stats);
}
