byte is_using_flecs_stats = 1;

void initialize_flecs_profiler(ecs* world) {
    (void) world;
    //if (is_using_flecs_stats) {
/*#ifdef zox_use_flecs_profiler
        zox_log("Flecs Profiler Enabled");
        zox_import_module(FlecsStats);
        ecs_measure_system_time(world, 1);
        ecs_measure_frame_time(world, 1);

        //ECS_IMPORT(world, FlecsMonitor);
        // ecs_tracing_enable(1);
        // ecs_log_set_level(0);
#else
    zox_logw("Profiler Define Missing.");
#endif*/
    //}
}