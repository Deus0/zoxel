zox_sys2(TextureUploadSystem) {
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
        if (!data->length) {
            //const byte* values = (byte[]) { 0, 0, 0, 0 };  // RGBA all zero
            //zox_gpu_set_texture_color_rgba(gpu_link->value, int2_one, values);
            zox_gpu_clear_texture_rgba(gpu_link->value);
            if (dbg_log) {
                zox_log("Texture RGBA Uploaded [%s] [Cleared] GPU [%i]", zox_get_name(e), gpu_link->value);
            }
        } else if (data->length != size->value.x * size->value.y) {
            if (dbg_log) {
                zox_logw("Texture RGBA Size Wrong [%s] s[%ix%i] -> l[%i]", zox_get_name(e), size->value.x, size->value.y, data->length);
            }
        } else if (data->length) {
            // if data and lengths all valid
            zox_gpu_set_texture_color_rgba(gpu_link->value, size->value, data->value);
            if (dbg_log) {
                zox_log("Texture Uploaded [%s] Size [%ix%i] GPULink [%i]", zox_get_name(e), size->value.x, size->value.y, gpu_link->value);
            }
        }
    }
} zox_sys_end(TextureUploadSystem);
