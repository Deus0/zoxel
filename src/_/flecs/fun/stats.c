#ifdef FLECS_STATS

typedef struct {
    const char *name;
    double avg_ms;
    double min_ms;
    double max_ms;
    int32_t matched;
    bool task;
} profile_entry_t;

static double metric_ms(ecs_metric_t m, int32_t t)
{
    return m.gauge.avg[t] * 1000.0;
}

static double metric_min_ms(ecs_metric_t m, int32_t t)
{
    return m.gauge.min[t] * 1000.0;
}

static double metric_max_ms(ecs_metric_t m, int32_t t)
{
    return m.gauge.max[t] * 1000.0;
}

void log_pipelines(ecs_world_t *world)
{
    ecs_entity_t pipeline_id = ecs_get_pipeline(world);
    ecs_pipeline_stats_t p = {0};
    if (!pipeline_id || !ecs_pipeline_stats_get(world, pipeline_id, &p)) {
        printf("No pipeline stats available.\n");
        return;
    }
    ecs_entity_t *systems = ecs_vec_first_t(&p.systems, ecs_entity_t);
    int32_t system_count = ecs_vec_count(&p.systems);
    ecs_sync_stats_t *syncs = ecs_vec_first_t(&p.sync_points, ecs_sync_stats_t);
    int32_t sync_count = ecs_vec_count(&p.sync_points);
    printf("\n=== Flecs pipeline stats ===\n");
    printf("pipeline: %s\n", ecs_get_name(world, pipeline_id) ? ecs_get_name(world, pipeline_id) : "<unnamed>");
    printf("systems: %d  active: %d  sync_points: %d  rebuilds: %d\n",
            p.system_count, p.active_system_count, sync_count, p.rebuild_count);
    if (sync_count && syncs) {
        printf("\n-- sync points --\n");
        for (int32_t i = 0; i < sync_count; i++) {
            printf("%2d. %8.3f ms  systems_before=%d  mt=%d  immediate=%d\n",
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
            ecs_system_stats_t s = {0};
            if (!ecs_system_stats_get(world, sys, &s)) {
                continue;
            }
            const char *name = ecs_get_name(world, sys);
            printf("%-40s %8.3f ms%s\n",
                    name ? name : "<unnamed>",
                    metric_ms(s.time_spent, p.t),
                    s.task ? "  [task]" : "");
        }
    }
    ecs_pipeline_stats_fini(&p);
}


static int compare_profile(const void *a, const void *b) {
    const profile_entry_t *pa = a;
    const profile_entry_t *pb = b;
    if (pa->avg_ms < pb->avg_ms) return 1;
    if (pa->avg_ms > pb->avg_ms) return -1;
    return 0;
}

void log_system_stats(ecs* world) {
    ecs_entity_t pipeline_id = ecs_get_pipeline(world);
    ecs_pipeline_stats_t p = {0};
    if (!pipeline_id || !ecs_pipeline_stats_get(world, pipeline_id, &p)) {
        return;
    }
    ecs_entity_t *systems = ecs_vec_first_t(&p.systems, ecs_entity_t);
    int32_t system_count = ecs_vec_count(&p.systems);
    profile_entry_t *entries = zox_malloc(sizeof(profile_entry_t) * system_count);
    int entry_count = 0;
    for (int32_t i = 0; i < system_count; i++) {
        ecs_entity_t sys = systems[i];
        if (!sys) {
            continue;
        }
        ecs_system_stats_t s = {0};
        if (!ecs_system_stats_get(world, sys, &s)) {
            continue;
        }
        entries[entry_count].name = ecs_get_name(world, sys);
        entries[entry_count].avg_ms = metric_ms(s.time_spent, p.t);
        entries[entry_count].min_ms = metric_min_ms(s.time_spent, p.t);
        entries[entry_count].max_ms = metric_max_ms(s.time_spent, p.t);
        // entries[entry_count].matched = s.query.result_count;
        entries[entry_count].matched = (int32_t) s.query.result_count.gauge.avg[p.t];
        entries[entry_count].task = s.task;
        entry_count++;
    }
    qsort(entries, entry_count, sizeof(profile_entry_t), compare_profile);
    printf("\n============================================================\n");
    printf("Frame %u\n", ecs_run_count);
    printf("------------------------------------------------------------\n");
    //printf("Pre Update : %7.3f ms\n", pre_ms);
    //printf("ECS        : %7.3f ms\n", ecs_ms);
    //printf("Post Update: %7.3f ms\n", post_ms);
    // printf("Total      : %7.3f ms\n", pre_ms + ecs_ms + post_ms);
    printf("\nPipeline\n");
    printf("Systems: %d  Active: %d  Sync Points: %d\n",
           p.system_count,
           p.active_system_count,
           ecs_vec_count(&p.sync_points));
    printf("\nTop Systems\n");
    printf("%-36s %8s %8s %8s %8s\n", "System", "Avg", "Min",  "Max", "Match");
    int max_print = entry_count < 15 ? entry_count : 15;
    for (int i = 0; i < max_print; i++) {
        printf("%-36.36s %8.3f %8.3f %8.3f %8d%s\n",
               entries[i].name ? entries[i].name : "<unnamed>",
               entries[i].avg_ms,
               entries[i].min_ms,
               entries[i].max_ms,
               entries[i].matched,
               entries[i].task ? " T" : "");
    }
    free(entries);
    ecs_pipeline_stats_fini(&p);
}

void debug_ecs_stats(ecs *world) {
    if (zox_log_system_stats) {
        log_system_stats(world);
    }
    if (zox_log_pipelines) {
        log_pipelines(world);
    }
}

#endif
