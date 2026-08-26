extern double current_time_in_seconds();

void update_ecs(ecs *world) {
    /*byte dbg_log = 0;
    double pre_1 = current_time_in_seconds();
    run_update_loop(world);
    double time_1 = current_time_in_seconds() - pre_1;
    if (flecs_log_level) {
        ecs_log_set_level(flecs_log_level);
    }
    double pre_2 = current_time_in_seconds();
    ecs_progress(world, 0);
    double time_2 = current_time_in_seconds() - pre_2;
    double pre_3 = current_time_in_seconds();
    run_post_update_loop(world);
    double time_3 = current_time_in_seconds() - pre_3;
    if (dbg_log) {
        zox_log("Time 1[%fms] 2[%fms] 3[%fms]", time_1 * 1000, time_2 * 1000, time_3 * 1000);
    }*/
    byte dbg_log = 0;
    run_update_loop(world);
    static double last_merge = 0;
    static double last_rematch = 0;
    double t0 = current_time_in_seconds();
    ecs_progress(world, 0);
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
    run_post_update_loop(world);
    ecs_run_count++;
#ifdef FLECS_STATS
    debug_ecs_stats(world);
#endif
/*#ifdef FLECS_PROFILER
    debug_print_ecs_frame(world);
#endif*/
}

void update_ecs_local() {
    update_ecs(local_world);
}
