#ifdef zox_logs
    extern double current_time_in_seconds();
#endif

void update_ecs(ecs *world) {
    if (flecs_log_level) {
        ecs_log_set_level(flecs_log_level);
    }
    byte dbg_log = 0;
    // run_update_loop(world);
#ifdef zox_logs
    static double last_merge = 0;
    static double last_rematch = 0;
    double t0 = current_time_in_seconds();
#endif
    ecs_progress(world, 0);
#ifdef zox_logs
    double t1 = current_time_in_seconds();
    const ecs_world_info_t *info = ecs_get_world_info(world);
    double merge = info->merge_time_total - last_merge;
    double rematch = info->rematch_time_total - last_rematch;
    last_merge = info->merge_time_total;
    last_rematch = info->rematch_time_total;
    if (dbg_log) {
        zox_log(
            "FLECS %.3f ms | merge %.3f ms | rematch %.3f ms",
            (t1 - t0) * 1000.0,
            merge * 1000.0,
            rematch * 1000.0
        );
    }
#endif
#ifdef flecs_profiler
    if (zox_log_system_stats) {
        log_system_stats(world);
    }
    if (zox_log_pipelines) {
        log_pipelines(world, zox_log_pipelines);
    }
#endif
    ecs_run_count++;
}

void update_ecs_local() {
    update_ecs(local_world);
}
