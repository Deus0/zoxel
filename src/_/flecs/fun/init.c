void initialize_threads(ecs* world, byte cores) {
    if (cores > 1 && !zox_disable_threads) {
        zox_logv("Enabling Threads [%i]", cores);
        ecs_set_threads(world, cores);
    } else {
        zox_logv("Single Threads Enabled");
        ecs_set_threads(world, 0);
    }
}

ecs* real_world;

void initialize_ecs_settings(ecs *world, byte fps, byte cores) {
    real_world = world;
    initialize_threads(world, cores);
    ecs_set_target_fps(world, (float) fps);
#ifdef flecs_profiler
    zox_log("Enabled [flecs_profiler]");
    ECS_IMPORT(world, FlecsStats);
    ECS_IMPORT(world, FlecsRest);
    ecs_measure_frame_time(world, 1);
    ecs_measure_system_time(world, 1);
    ecs_singleton_set(world, EcsRest, {0});
#endif

}
