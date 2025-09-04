void camera_render_update(iter *it, const byte is_camera2D) {
    byte do_renders = !headless && rendering;
    if (!do_renders) {
        return;
    }
    opengl_set_defaults(!is_camera2D);

    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ViewMatrix);
    zox_sys_in(Position3D);
    zox_sys_in(FieldOfView);
    zox_sys_in(ScreenPosition);
    zox_sys_in(ScreenDimensions);
    zox_sys_in(FogColor);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ViewMatrix, matrix);
        zox_sys_i(Position3D, position);
        zox_sys_i(FieldOfView, fov);
        zox_sys_i(ScreenPosition, screen_position);
        zox_sys_i(ScreenDimensions, screen_size);
        zox_sys_i(FogColor, fog);

        // pass these data to systems
        renderer_camera = e;
        render_camera_fov = fov->value;
        render_camera_matrix = matrix->value;
        render_camera_position = position->value;
        renderer_fog_color = fog->value;

        // set render objects
        uint fbo = 0;
        if (!is_camera2D) {
            if (zox_has(e, FrameBufferLink)) {
                fbo = zox_get_value(e, FrameBufferLink)
            }
        }
        if (render_backend == zox_render_backend_opengl) {
            if (fbo) {
                // rendering to texture
                glViewport(
                    0,
                    0,
                    screen_size->value.x / 1,
                    screen_size->value.y / 1);
            } else {
                glViewport(
                    screen_position->value.x,
                    screen_position->value.y,
                    screen_size->value.x,
                    screen_size->value.y);
            }
        }
        // todo: this required but breaks it for both render cameras
        if (fbo) {
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
            #ifdef zoxel_catch_opengl_errors
            if (!check_opengl_frame_buffer_status()) {
                zox_log(" !! camera render - error on fbo [%u]\n", fbo)
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                continue;
            }
            #endif
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        }
        #ifdef zox_vulkan
        // else { set vulkan viewport; }
        #endif
        if (!is_camera2D) {
            for (size_t j = 0; j < render3D_systems->size; j++) {
                ecs_run(world, render3D_systems->data[j], 0, NULL);
            }
            for (size_t j = 0; j < render3D_plus_systems->size; j++) {
                ecs_run(world, render3D_plus_systems->data[j], 0, NULL);
            }
            // zox_log("+ [%s] rendering with: [%ix%i]", zox_get_name(e), screenDimensions->value.x, screenDimensions->value.y)
        } else {
            for (renderer_layer = 0; renderer_layer < max_layers2D; renderer_layer++) { // ui rendered in layers
                for (size_t j = 0; j < render2D_systems->size; j++) {
                    ecs_run(world, render2D_systems->data[j], 0, NULL);
                }
            }
        }
        if (fbo) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    }
}