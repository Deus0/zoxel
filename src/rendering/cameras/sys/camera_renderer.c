static inline void opengl_begin_camera(byte is_3D) {
    if (is_3D) {
        zox_gpu_enable_depth_test();        // cull for 3D things
        zox_gpu_enable_culling();
        zox_gpu_disable_blend();
    } else {
        zox_gpu_disable_depth_test();
        zox_gpu_disable_culling();
        zox_gpu_disable_blend();
    }
}

void camera_render_update(iter *it, byte is_camera2D) {
    byte dbg_log = 0;
    byte do_renders = rendering;
    if (!do_renders) {
        return;
    }
    opengl_begin_camera(!is_camera2D);
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
                zox_gpu_set_viewport(float2_zero, int2_to_float2(screen_size->value));
            } else {
                zox_gpu_set_viewport(int2_to_float2(screen_position->value),  int2_to_float2(screen_size->value));
            }
        }
        // todo: this required but breaks it for both render cameras
        if (fbo) {
            zox_gpu_bind_fbo(fbo);
#ifdef zoxel_catch_opengl_errors
            if (!check_opengl_frame_buffer_status()) {
                zox_log_error("Camera render - error on fbo [%u]", fbo);
                zox_gpu_bind_fbo(0);
                continue;
            }
#endif
            zox_gpu_clear_viewport();
        }
        #ifdef zox_vulkan
        // else { set vulkan viewport; }
        #endif
        if (!is_camera2D) {
            if (dbg_log) {
                zox_log("Camera Rendering [%s]: [%ix%i]", zox_get_name(e), screen_size->value.x, screen_size->value.y);
            }
            // TODO: Tag systems as Render2D systems and use tag here
            for (byte j = 0; j < max_render_loop_orders; j++) {
                for (size_t k = 0; k < render3D_systems->size; k++) {
                    entity system = render3D_systems->data[k];
                    byte order = zox_has(system, RenderOrder) ? zox_getv(system, RenderOrder) : 0;
                    if (order == j) {
                        if (dbg_log) {
                            zox_log(" - %i [%s] (%i)", k, zox_get_name(system), j);
                        }
                        ecs_run(world, system, 0, NULL);
                    }
                }
            }
        } else {
            if (zox_new_ui_renderer) {
                for (size_t j = 0; j < render2D_systems->size; j++) {
                    entity system = render2D_systems->data[j];
                    ecs_run(world, system, 0, NULL);
                }
            } else {
                for (renderer_layer = 0; renderer_layer < max_layers2D; renderer_layer++) {
                    for (size_t j = 0; j < render2D_systems->size; j++) {
                        entity system = render2D_systems->data[j];
                        ecs_run(world, system, 0, NULL);
                    }
                }
            }
        }
        if (fbo) {
            zox_gpu_bind_fbo(0);
        }
    }
}
