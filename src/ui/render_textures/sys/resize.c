zox_sys2(RenderTextureSizeSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LayoutSizeDirty);
    zox_sys_in(TextureSize);
    zox_sys_in(TextureGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(LayoutSizeDirty, dirty);
        zox_sys_i(TextureSize, size);
        zox_sys_i(TextureGPULink, texture_gpu);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        if (!texture_gpu->value) {
            zox_loge("[RenderTextureSizeSystem] [%s] Invalid [TextureGPULink]", zox_getn(e));
            continue;
        }
        entity camera = zox_get_link(world, e, CameraLink);
        if (!zox_valid(camera)) {
            zox_loge("[RenderTextureSizeSystem] Invalid Render Camera [%lu]",
                camera);
            continue;
        }
        if (!zox_has(camera, RenderBufferLink)) {
            zox_loge("[RenderTextureSizeSystem] Invalid Render Camera: No RenderBufferLink [%s]", zox_getn(camera));
            continue;
        }
        guint rbo = zox_getv(camera, RenderBufferLink);
        int2 scaled_size = scale_viewport(size->value);
        set_render_texture_gpu(
            texture_gpu->value,
            size->value,
            zox_has(e, RenderTextureAlpha));
        zox_gpu_set_rbo_size(rbo, size->value);
        if (dbg_log) {
            zox_log("+ [%s] Render Scaled Size: %ix%i - og [%ix%i]", zox_get_name(e), scaled_size.x, scaled_size.y, size->value.x, size->value.y);
        }
    }
} zox_sys_end(RenderTextureSizeSystem);
