extern double current_time_in_seconds();

void update_ecs(ecs *world) {
    byte dbg_log = 0;
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
    }
    ecs_run_count++;
#ifdef FLECS_STATS
    if ((ecs_run_count % 60) == 0) {
        debug_ecs_stats(world);
    }
#endif
#ifdef FLECS_PROFILER
    debug_print_ecs_frame(world);
#endif
}

void update_ecs_local() {
    update_ecs(local_world);
}
