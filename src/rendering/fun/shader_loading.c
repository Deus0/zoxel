zox_hook(load_shader, (ecs* world), (world))

void process_shaders(ecs *world) {
    if (!rendering_initialized) {
        zox_log_error("Rendering is not enabled.");
        //exit(1);
    } else {
        run_hook_load_shader(world);
    }
}

byte get_new_shader_source_index() {
    shaders_count++;
    return shaders_count - 1;
}
