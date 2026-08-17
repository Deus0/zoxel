int_array_d* gpu_dispose_systems;
int_array_d* gpu_restore_systems;

void initialize_gpu_systems() {
    gpu_dispose_systems = create_int_array_d(initial_dynamic_array_size);
    gpu_restore_systems = create_int_array_d(initial_dynamic_array_size);
}

void dispose_gpu_systems() {
    dispose_int_array_d(gpu_dispose_systems);
    dispose_int_array_d(gpu_restore_systems);
}

void add_to_gpu_dispose_systems(long int id) {
    int_array_d_add(gpu_dispose_systems, id);
}

void add_to_gpu_restore_systems(long int id) {
    int_array_d_add(gpu_restore_systems, id);
}

void run_gpu_dispose_systems(ecs *world) {
    for (size_t i = 0; i < gpu_dispose_systems->size; i++) {
        entity e = gpu_dispose_systems->data[i];
        if (!zox_valid(e)) {
            zox_loge("Invalid Dispose System [%s]", zox_getn(e));
            continue;
        }
        ecs_run(world, e, 0, NULL);
    }
}

void run_gpu_restore_systems(ecs *world) {
    for (size_t i = 0; i < gpu_restore_systems->size; i++) {
        entity e = gpu_restore_systems->data[i];
        if (!zox_valid(e)) {
            zox_loge("Invalid Restore System [%s]", zox_getn(e));
            continue;
        }
        ecs_run(world, e, 0, NULL);
    }
}

byte did_dispose_resources = 0;

void opengl_dispose_resources(ecs *world) {
    if (!did_dispose_resources) {
        did_dispose_resources = 1;
        // zox_log("Disposing all opengl resources");
        rendering = 0;
        minimized = 1; // move this to a system and function for app! when implement multi apps
        updating_time = 0; // timesteps a bit low atm so pause while minimizing
        run_gpu_dispose_systems(world);
    }
}

void opengl_restore_resources(ecs *world) {
    if (did_dispose_resources) {
        did_dispose_resources = 0;
        // zox_log("Restoring all opengl resources");
        run_gpu_restore_systems(world);
        rendering = 1;
        minimized = 0;
        updating_time = 1;
        skip_time_to_current();
    }
}
