// RenderTexture
void RenderTextureSizeSystem(iter *it) {

    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LayoutSizeDirty);
    zox_sys_in(PixelSize);
    zox_sys_in(TextureGPULink);
    zox_sys_in(CameraLink);
    // zox_sys_out(TextureSize);

    for (int i = 0; i < it->count; i++) {

        zox_sys_e()
        zox_sys_i(LayoutSizeDirty, dirty);
        zox_sys_i(PixelSize, layout_size);
        zox_sys_i(TextureGPULink, texture_gpu);
        zox_sys_i(CameraLink, camera);
        // zox_sys_o(TextureSize, texture_size);

        if (dirty->value == zox_dirty_active) {
            continue;
        }

        const int2 scaled_size = scale_viewport(layout_size->value);
        // zox_set(e, TextureSize, { scaled_size })
        set_render_texture_gpu(texture_gpu->value, scaled_size);

        set_render_buffer_size(
            zox_gett_value(camera->value, RenderBufferLink),
            scaled_size);

    }
} zoxd_system2(RenderTextureSizeSystem);