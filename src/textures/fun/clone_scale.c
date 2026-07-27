void clone_texture_data_scale(ecs *world, entity e, entity src, int2 new_size) {
    if (!src || !zox_has(src, TextureSize) || !zox_has(src, TextureData)) {
        if (!src) {
            zox_loge("[texture not found] [%s]", zox_get_name(e));
        } else {
            zox_loge("[texture invalid] [%s] > source [%s]", zox_get_name(e), zox_get_name(src));
        }
        return;
    }
    zox_geter(src, TextureData, source_data);
    if (source_data->length == 0 || !source_data->value) {
        zox_loge("[clone_texture_data] Invalid src TextureData [%s] > source [%s]", zox_get_name(e), zox_get_name(src));
        return;
    }
    zox_geter_value(src, TextureSize, int2, original_size);
    /* basic sanity */
    if (original_size.x <= 0 || original_size.y <= 0 || new_size.x <= 0 || new_size.y <= 0) {
        zox_loge("[clone_texture_data] invalid sizes src=%dx%d dst=%dx%d", original_size.x, original_size.y, new_size.x, new_size.y);
        return;
    }
    TextureData data = { 0 };
    /* allocate destination buffer (element count = width * height) */
    initialize_TextureData(&data, new_size.x * new_size.y);
    if (!data.value) {
        zox_log_error("texture data malloc failed");
        return;
    }
    /* Nearest-neighbor scale.
       Assumption: pixel is 32-bit (RGBA). If your pixel type differs, change pixel_t. */
    typedef uint32_t pixel_t;
    const pixel_t *src_pixels = (const pixel_t*)source_data->value;
    pixel_t *dst_pixels = (pixel_t*)data.value;
    int orig_w = original_size.x;
    int orig_h = original_size.y;
    int dst_w = new_size.x;
    int dst_h = new_size.y;
    /* If sizes are identical, memcpy full buffer (fast path) */
    if (orig_w == dst_w && orig_h == dst_h) {
        size_t count = (size_t)orig_w * (size_t)orig_h;
        memcpy(dst_pixels, src_pixels, count * sizeof(pixel_t));
    } else {
        for (int y = 0; y < dst_h; ++y) {
            /* map destination y to source y */
            int src_y = (int)(( (long long)y * orig_h) / dst_h);
            if (src_y >= orig_h) src_y = orig_h - 1;
            for (int x = 0; x < dst_w; ++x) {
                int src_x = (int)(( (long long)x * orig_w) / dst_w);
                if (src_x >= orig_w) src_x = orig_w - 1;
                dst_pixels[(size_t)y * dst_w + x] = src_pixels[(size_t)src_y * orig_w + src_x];
            }
        }
    }
    zox_set_ptr(e, TextureData, data);
    zox_set(e, TextureSize, { new_size });
    zox_set(e, TextureDirty, { zox_upload_texture });
    // zox_log("cloned texture data %s => %s", zox_get_name(e), zox_get_name(src));
}
