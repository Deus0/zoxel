zox_sys2(TextureRgbaUploadSystem) {
    byte dbg_log = 0;
    byte dbg_save = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TextureData);
    zox_sys_in(TextureSize);
    zox_sys_in(TextureGPULink);
    zox_sys_out(TextureDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TextureData, data);
        zox_sys_i(TextureSize, size);
        zox_sys_i(TextureGPULink, gpu_link);
        zox_sys_o(TextureDirty, dirty);
        if (dirty->value != zox_upload_texture) {
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
        dirty->value = 0;
    }
} zox_sys_end(TextureRgbaUploadSystem);

static inline void zox_gpu_clear_texture_rgb(guint id) {
    zox_gpu_bind_texture(id);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        0,
        0,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        NULL);
    zox_gpu_bind_texture(0);
}
// TextureRGB's
zox_sys2(TextureRgbUploadSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TextureData);
    zox_sys_in(TextureSize);
    zox_sys_in(TextureGPULink);
    zox_sys_out(TextureDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TextureData, data);
        zox_sys_i(TextureSize, size);
        zox_sys_i(TextureGPULink, gpu_link);
        zox_sys_o(TextureDirty, dirty);
        if (dirty->value != zox_upload_texture) {
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
        dirty->value = 0;
}
} zox_sys_end(TextureRgbUploadSystem);
