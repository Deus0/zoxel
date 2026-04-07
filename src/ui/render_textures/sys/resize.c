zox_sys2(RenderTextureSizeSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LayoutSizeDirty);
    zox_sys_in(TextureSize);
    zox_sys_in(TextureGPULink);
    zox_sys_in(CameraLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(LayoutSizeDirty, dirty);
        zox_sys_i(TextureSize, tsize);
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

        if (!zox_valid(camera->value)) {
            zox_logw("Invalid Render Camera");
            continue;
        }

        if (!zox_has(camera->value, RenderBufferLink)) {
            zox_log_error("Render Camera has no RenderBufferLink [%s]", zox_get_name(camera->value));
            continue;
        }

        zox_geter_value(camera->value, RenderBufferLink, uint, rbo);

        // int2 scaled_size = scale_viewport(size->value);

        set_render_texture_gpu(texture_gpu->value, tsize->value);
        set_render_buffer_size(rbo, tsize->value);

        /*zox_sys_e();
        zox_log("+ [%s] Render Scaled Size: %ix%i - og [%ix%i]", zox_get_name(e), scaled_size.x, scaled_size.y, size->value.x, size->value.y);*/

    }
} zox_sys_end(RenderTextureSizeSystem);
