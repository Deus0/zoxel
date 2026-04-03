zox_sys2(TextureUpdateSystem) {
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

        glBindTexture(GL_TEXTURE_2D, gpu_link->value);

        if (!data->length) {
            const byte values[4] = { 0, 0, 0, 0 };  // RGBA all zero
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, values);

            // zox_log("+ rgba [%s] cleared texture", zox_get_name(e));
        }
        else if (data->length != size->value.x * size->value.y) {
            // zox_logw("[%s] Invalid TextureRGBA Size s[%ix%i] -> l[%i]", zox_get_name(e), size->value.x, size->value.y, data->length);
            glBindTexture(GL_TEXTURE_2D, 0);
            continue;
        }
        // if data and lengths all valid
        else if (data->length) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size->value.x, size->value.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data->value);

            // zox_log("+ rgba uploaded [%s] size [%ix%i]", zox_get_name(e), size->value.x, size->value.y);
        }

        glBindTexture(GL_TEXTURE_2D, 0);
    }
} zox_sys_end(TextureUpdateSystem);
