// TextureRGB's
zox_sys2(TextureRgbUploadSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TextureDirty);
    zox_sys_in(TextureData);
    zox_sys_in(TextureSize);
    zox_sys_in(TextureGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TextureDirty, dirty);
        zox_sys_i(TextureData, data);
        zox_sys_i(TextureSize, size);
        zox_sys_i(TextureGPULink, gpu_link);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        if (!gpu_link->value) {
            zox_logw("[%s] Invalid GPU Link [0]", zox_get_name(e));
            continue;
        }
        if (data->value) {
            if (data->length == size->value.x * size->value.y) {
                zox_gpu_set_texture_color_rgb(gpu_link->value, size->value, data->value);
            } else {
                zox_gpu_bind_texture(0);
            }
        } else {
            const byte* values = (byte[]) { 0, 0, 0 };  // RGBA all zero
            zox_gpu_set_texture_color_rgb(gpu_link->value, int2_one,values);
            if (dbg_log) {
                zox_log("+ [%s] cleared texture", zox_get_name(e));
            }
        }
    }
} zox_sys_end(TextureRgbUploadSystem);
