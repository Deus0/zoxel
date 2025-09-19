// TextureRGB's
void TextureRGBUpdateSystem(iter *it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TextureDirty);
    zox_sys_in(TextureData);
    zox_sys_in(TextureSize);
    zox_sys_in(TextureGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e()
        zox_sys_i(TextureDirty, dirty)
        zox_sys_i(TextureData, data)
        zox_sys_i(TextureSize, size)
        zox_sys_i(TextureGPULink, gpu_link)

        if (dirty->value != zox_dirty_active) {
            continue;
        }

        if (!gpu_link->value) {
            zox_logw("[%s] Invalid GPU Link [0]", zox_get_name(e));
            continue;
        }

        glBindTexture(GL_TEXTURE_2D, gpu_link->value);

        if (data->value) {
            if (data->length == size->value.x * size->value.y) {
                // different to above
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size->value.x, size->value.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
                // zox_log("+ [%s] uploaded texture [%ix%i]", zox_get_name(e), size->value.x, size->value.y);
            } else {
                // zox_logw("[%s] Invalid TextureRGB Size s[%ix%i] -> l[%i]", zox_get_name(e), size->value.x, size->value.y, data->length);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
        } else {
            const byte values[3] = { 0, 0, 0 };  // RGBA all zero
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, values);
            // zox_log("+ [%s] cleared texture", zox_get_name(e));
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        // zox_log_error("+ uploaded [%s] size [%ix%i]", zox_get_name(e), size->value.x, size->value.y)
    }
} zoxd_system(TextureRGBUpdateSystem)