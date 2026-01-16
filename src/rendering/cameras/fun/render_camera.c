void create_camera_rbo_and_fbo(ecs* world, entity e, int2 size) {
    if (!zox_use_post_processing) {
        return;
    }

    uint fbo = spawn_frame_buffer_object(world, e);
    uint rbo = spawn_render_buffer(world, e, size);

    if (fbo && rbo) {
        connect_render_buffer_to_fbo(fbo, rbo);
    }
}
