zox_sys2(TextureRgbaUploadSystem) {
    byte dbg_log = 0;
    byte max_process = !zox_disable_process_skips ? texture_upload_rate : 0;
    byte dbg_save = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TextureData);
    zox_sys_in(TextureSize);
    zox_sys_in(TextureGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TextureData, data);
        zox_sys_i(TextureSize, size);
        zox_sys_i(TextureGPULink, gpu_link);
        if (max_process && process_count > max_process) {
            continue;
        }
        if (!gpu_link->value) {
            // zox_loge("Invalid [TextureGPULink] (RGBA) on  [%s]", zox_sys_e_name);
            // dirty->value = 0;
            continue;
        }
        if (!data->length) {
            zox_gpu_clear_texture_rgba(gpu_link->value);
            if (dbg_log) {
                zox_log("Texture RGBA Cleared [%s] GPU [%i]", zox_get_name(e), gpu_link->value);
            }
        } else if (data->length != size->value.x * size->value.y) {
            if (dbg_log) {
                zox_logw("Texture RGBA Size Wrong [%s] s[%ix%i] -> l[%i]", zox_get_name(e), size->value.x, size->value.y, data->length);
            }
        } else if (data->length) {
            // if data and lengths all valid
            zox_gpu_set_texture_color_rgba(
                gpu_link->value,
                size->value,
                data->value);
            if (dbg_log) {
                uint32_t checksum = 0;
                for (int j = 0; j < data->length; j++) {
                    checksum += data->value[j].r;
                    checksum += data->value[j].g;
                    checksum += data->value[j].b;
                    checksum += data->value[j].a;
                }
                zox_log("Uploaded Texture RGBA [%s] Size [%ix%i] TextureGPULink [%i] checksum=%u", zox_get_name(e), size->value.x, size->value.y, gpu_link->value, checksum);
            }
            if (dbg_save) {
                char path[256];
                sprintf(path, "bin/dbg/texture_%s.bmp", zox_getn(e));
                save_texture_to_bmp(path, data, size->value);
            }
        }
        zox_remove(e, TextureDirty);
        zox_sys_increment();
    }
} zox_sys_end(TextureRgbaUploadSystem);

// TextureRGB's
zox_sys2(TextureRgbUploadSystem) {
    byte dbg_log = 0;
    byte max_process = !zox_disable_process_skips ? texture_upload_rate : 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TextureData);
    zox_sys_in(TextureSize);
    zox_sys_in(TextureGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TextureData, data);
        zox_sys_i(TextureSize, size);
        zox_sys_i(TextureGPULink, gpu_link);
        if (max_process && process_count > max_process) {
            continue;
        }
        if (!gpu_link->value) {
            // zox_logw("Invalid [TextureGPULink] (RGB) on [%s]", zox_get_name(e));
            // dirty->value = 0;
            continue;
        }
        if (!data->length) {
            zox_gpu_clear_texture_rgb(gpu_link->value);
        } else if (data->length != size->value.x * size->value.y) {
            if (dbg_log) {
                zox_logw(
                    "Texture RGB Size Wrong [%s] s[%ix%i] -> l[%i]",
                    zox_get_name(e),
                    size->value.x,
                    size->value.y,
                    data->length);
            }
        } else {
            zox_gpu_set_texture_color_rgb(
                gpu_link->value,
                size->value,
                data->value);
        }
        zox_remove(e, TextureDirty);
        zox_sys_increment();
}
} zox_sys_end(TextureRgbUploadSystem);
