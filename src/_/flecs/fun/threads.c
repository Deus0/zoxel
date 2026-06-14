void initialize_threads(ecs* world, byte cores) {
    zox_logv("Setting Threads [%i]", cores);
    if (cores > 1 && is_multithreading) {
        ecs_set_threads(world, cores);
    } else {
        zox_logw("Single Threads Enabled");
        ecs_set_threads(world, 0);
    }
}

ecs* real_world;

void initialize_ecs_settings(ecs *world, float fps, byte cores) {
    real_world = world;
    initialize_threads(world, cores);
    ecs_set_target_fps(world, fps);
}