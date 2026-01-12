void RenderTextureSizeSystem(iter *it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LayoutSizeDirty);
    zox_sys_in(LayoutSize);
    zox_sys_in(TextureGPULink);
    zox_sys_in(CameraLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(LayoutSizeDirty, dirty);
        zox_sys_i(LayoutSize, size);
        zox_sys_i(TextureGPULink, texture_gpu);
        zox_sys_i(CameraLink, camera);

        if (dirty->value != zox_dirty_active) {
            continue;
        }

        if (!texture_gpu->value) {
            zox_sys_e();
            zox_logw("[%s] Failure [RenderTextureSizeSystem]", zox_get_name(e));
            continue;
        }

        const int2 scaled_size = scale_viewport(size->value);
        zox_geter_value(camera->value, RenderBufferLink, uint, rbo);
        set_render_texture_gpu(
            texture_gpu->value,
            scaled_size
        );
        set_render_buffer_size(
            rbo,
            scaled_size
        );

        /*zox_sys_e();
        zox_log("+ [%s] Render Scaled Size: %ix%i - og [%ix%i]", zox_get_name(e), scaled_size.x, scaled_size.y, size->value.x, size->value.y);*/

    }
} zoxd_system2(RenderTextureSizeSystem);