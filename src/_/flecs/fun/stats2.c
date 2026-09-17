#ifdef flecs_profiler

typedef struct {
    const char *name;
    double ms;
    int32_t matched;
    bool task;
} profile_entry_t;

typedef struct {
    ecs_entity_t entity;
    ecs_ftime_t time_spent;
} system_time_entry_t;

static system_time_entry_t *previous_system_times = NULL;
static int32_t previous_system_count = 0;

static double metric_ms(ecs_metric_t m, int32_t t)
{
    return m.gauge.avg[t] * 1000.0;
}

static int compare_profile(const void *a, const void *b)
{
    const profile_entry_t *pa = a;
    const profile_entry_t *pb = b;

    if (pa->ms < pb->ms) return 1;
    if (pa->ms > pb->ms) return -1;
    return 0;
}

static ecs_ftime_t get_previous_system_time(ecs_entity_t system)
{
    for (int32_t i = 0; i < previous_system_count; i++) {
        if (previous_system_times[i].entity == system) {
            return previous_system_times[i].time_spent;
        }
    }
    return 0;
}

static void set_previous_system_time(
    ecs_entity_t system,
    ecs_ftime_t time_spent)
{
    for (int32_t i = 0; i < previous_system_count; i++) {
        if (previous_system_times[i].entity == system) {
            previous_system_times[i].time_spent = time_spent;
            return;
        }
    }

    system_time_entry_t *new_entries = realloc(
        previous_system_times,
        sizeof(system_time_entry_t) * (previous_system_count + 1));

    if (!new_entries) {
        return;
    }

    previous_system_times = new_entries;

    previous_system_times[previous_system_count].entity = system;
    previous_system_times[previous_system_count].time_spent = time_spent;
    previous_system_count++;
}

void log_pipelines(ecs_world_t *world, byte dbg_level) {
    ecs_entity_t pipeline_id = ecs_get_pipeline(world);
    ecs_pipeline_stats_t p = {0};

    if (!pipeline_id ||
        !ecs_pipeline_stats_get(world, pipeline_id, &p))
    {
        printf("No pipeline stats available.\n");
        return;
    }

    ecs_entity_t *systems =
        ecs_vec_first_t(&p.systems, ecs_entity_t);

    int32_t system_count =
        ecs_vec_count(&p.systems);

    ecs_sync_stats_t *syncs =
        ecs_vec_first_t(&p.sync_points, ecs_sync_stats_t);

    int32_t sync_count =
        ecs_vec_count(&p.sync_points);

    printf("\n=== Flecs pipeline stats ===\n");

    const char *pipeline_name =
        ecs_get_name(world, pipeline_id);

    printf(
        "pipeline: %s\n",
        pipeline_name ? pipeline_name : "<unnamed>");

    printf(
        "systems: %d  active: %d  sync_points: %d  rebuilds: %d\n",
        p.system_count,
        p.active_system_count,
        sync_count,
        p.rebuild_count);

    if (sync_count && syncs) {
        printf("\n-- sync points --\n");

        for (int32_t i = 0; i < sync_count; i++) {
            printf(
                "%2d. %8.3f ms  systems_before=%d  mt=%d  immediate=%d\n",
                i + 1,
                metric_ms(syncs[i].time_spent, p.t),
                syncs[i].system_count,
                syncs[i].multi_threaded ? 1 : 0,
                syncs[i].immediate ? 1 : 0);
        }
    }

    if (system_count && systems) {
        printf("\n-- systems --\n");

        for (int32_t i = 0; i < system_count; i++) {
            ecs_entity_t sys = systems[i];

            if (!sys) {
                printf("---- merge ----\n");
                continue;
            }

            const ecs_system_t *system =
                ecs_system_get(world, sys);

            if (!system) {
                continue;
            }

            ecs_ftime_t previous =
                get_previous_system_time(sys);

            ecs_ftime_t current =
                system->time_spent;

            ecs_ftime_t frame_time =
                current - previous;

            if (frame_time < 0) {
                frame_time = current;
            }

            set_previous_system_time(sys, current);

            const char *name =
                ecs_get_name(world, sys);

            double ms = frame_time * 1000.0;

            if (dbg_level <= 1 && ms < 1) {
                continue;
            }

            printf(
                "%-40s %8.3f ms%s\n",
                name ? name : "<unnamed>",
                ms,
                system->run ? "  [task]" : "");
        }
    }

    ecs_pipeline_stats_fini(&p);
}

void log_system_stats(ecs *world)
{
    ecs_entity_t pipeline_id = ecs_get_pipeline(world);
    ecs_pipeline_stats_t p = {0};

    if (!pipeline_id ||
        !ecs_pipeline_stats_get(world, pipeline_id, &p))
    {
        return;
    }

    ecs_entity_t *systems =
        ecs_vec_first_t(&p.systems, ecs_entity_t);

    int32_t system_count =
        ecs_vec_count(&p.systems);

    profile_entry_t *entries =
        zox_malloc(sizeof(profile_entry_t) * system_count);

    int entry_count = 0;

    for (int32_t i = 0; i < system_count; i++) {
        ecs_entity_t sys = systems[i];

        if (!sys) {
            continue;
        }

        const ecs_system_t *system =
            ecs_system_get(world, sys);

        if (!system) {
            continue;
        }

        ecs_ftime_t previous =
            get_previous_system_time(sys);

        ecs_ftime_t current =
            system->time_spent;

        ecs_ftime_t frame_time =
            current - previous;

        if (frame_time < 0) {
            frame_time = current;
        }

        set_previous_system_time(sys, current);

        entries[entry_count].name =
            ecs_get_name(world, sys);

        entries[entry_count].ms =
            frame_time * 1000.0;

        entries[entry_count].matched = 0;
        entries[entry_count].task =
            system->run != NULL;

        entry_count++;
    }

    qsort(
        entries,
        entry_count,
        sizeof(profile_entry_t),
        compare_profile);

    printf(
        "\n============================================================\n");

    printf(
        "Frame %u\n",
        ecs_run_count);

    printf(
        "------------------------------------------------------------\n");

    printf(
        "Pipeline\n");

    printf(
        "Systems: %d  Active: %d  Sync Points: %d\n",
        p.system_count,
        p.active_system_count,
        ecs_vec_count(&p.sync_points));

    printf(
        "\nTop Systems - per frame\n");

    printf(
        "%-36s %10s\n",
        "System",
        "ms");

    int max_print =
        entry_count < 30 ? entry_count : 30;

    for (int i = 0; i < max_print; i++) {
        printf(
            "%-36.36s %10.3f%s\n",
            entries[i].name
                ? entries[i].name
                : "<unnamed>",
            entries[i].ms,
            entries[i].task ? " T" : "");
    }

    free(entries);

    ecs_pipeline_stats_fini(&p);
}

#endif