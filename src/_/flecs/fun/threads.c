void initialize_threads(ecs* world, byte cores) {
    zox_logv("Setting Threads [%i]", cores);
    if (cores > 1 && is_multithreading) {
        ecs_set_threads(world, cores);
    } else {
        zox_log("# warning, single threads set");
        ecs_set_threads(world, 0);
    }
}

void initialize_ecs_settings(ecs *world, float fps, byte cores) {
    initialize_threads(world, cores);
    ecs_set_target_fps(world, fps);
}