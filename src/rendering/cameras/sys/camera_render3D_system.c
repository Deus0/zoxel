// NOTE: NO need to time as it runs systems inside it
zox_sys_untimed(CameraRender3DSystem) {
    byte dbg_log = 0;
    if (!rendering) {
        return;
    }
    zox_gpu_enable_depth_test();        // cull for 3D things
    zox_gpu_enable_culling();
    zox_gpu_disable_blend();
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
        uint fbo = zox_has(e, FrameBufferLink) ? zox_getv(e, FrameBufferLink) : 0;
        if (fbo) {
            // rendering to texture
            zox_gpu_set_viewport(float2_zero, int2_to_float2(screen_size->value));
            zox_gpu_bind_fbo(fbo);
        } else {
            zox_gpu_set_viewport(int2_to_float2(screen_position->value),  int2_to_float2(screen_size->value));
        }
        color clear_color = zox_has(e, Color) ? zox_getv(e, Color) : color_black;
        zox_gpu_set_clear_color(color_to_float4(clear_color));
        zox_gpu_clear_viewport();
        if (dbg_log) {
            // zox_log("Camera [%s] Viewport Color is [%ix%ix%ix%i]", zox_getn(e), clear_color.r, clear_color.g, clear_color.b, clear_color.a);
            zox_log("Rendering 3D [%s]: [%ix%i] Buffer? [%i]",  zox_getn(e), screen_size->value.x, screen_size->value.y, fbo);
        }
        // TODO: Tag systems as Render2D systems and use tag here
        for (byte j = 0; j < max_render_loop_orders; j++) {
            for (size_t k = 0; k < render3D_systems->size; k++) {
                entity system = render3D_systems->data[k];
                byte order = zox_has(system, RenderOrder) ? zox_getv(system, RenderOrder) : 0;
                if (order == j) {
                    if (dbg_log >= 2) {
                        zox_log(" - %i [%s] (%i)", k, zox_getn(system), j);
                    }
                    ecs_run(world, system, 0, NULL);
                }
            }
        }
        if (fbo) {
            zox_gpu_reset_fbo();
        }
    }
} zox_sys_end_untimed(CameraRender3DSystem);