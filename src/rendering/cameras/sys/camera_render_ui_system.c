
zox_sys_untimed(CameraRenderUISystem) {
    byte dbg_log = 0;
    if (!rendering) {
        return;
    }
    zox_gpu_disable_depth_test();
    zox_gpu_disable_culling();
    zox_gpu_disable_blend();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ViewProjectionMatrix);
    zox_sys_in(Position3D);
    zox_sys_in(FieldOfView);
    zox_sys_in(ScreenPosition);
    zox_sys_in(PixelSize);
    zox_sys_in(FogColor);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ViewProjectionMatrix, matrix);
        zox_sys_i(Position3D, position);
        zox_sys_i(FieldOfView, fov);
        zox_sys_i(ScreenPosition, screen_position);
        zox_sys_i(PixelSize, screen_size);
        zox_sys_i(FogColor, fog);
        // pass these data to systems
        renderer_camera = e;
        render_camera_fov = fov->value;
        render_camera_matrix = matrix->value;
        render_camera_position = position->value;
        renderer_fog_color = fog->value;
        zox_gpu_set_viewport(int2_to_float2(screen_position->value),  int2_to_float2(screen_size->value));
        if (dbg_log) {
            zox_log("[%s] Rendering 2D [%s]: [%ix%i] Buffer? [%i]", zox_getn(e), screen_size->value.x, screen_size->value.y);
        }
        clear_ui_layers();
        for (size_t j = 0; j < render2D_systems->size; j++) {
            entity system = render2D_systems->data[j];
            ecs_run(world, system, 0, NULL);
        }
        render_uis(world, zox_ui_render_queue);
    }
} zox_sys_end_untimed(CameraRenderUISystem);