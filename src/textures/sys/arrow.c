void generate_texture_arrow(color* data, int2 size, color fill, color outline, byte thickness) {
    const color empty = color_clear;
    // Pass 1: generate arrow shape
    int center = size.x / 2;
    int half_height = size.y / 2;
    int shaft_width = size.x / 3;
    int index = 0;
    for (int y = 0; y < size.y; y++) {
        for (int x = 0; x < size.x; x++) {
            // Arrow head occupies upper half
            byte in_head = 0;
            if (y >= half_height) {
                int head_y = size.y - 1 - y;
                int half_width = (head_y * center) / (half_height - 1);
                in_head = (x >= center - half_width &&
                           x <= center + half_width);
            }
            // Arrow shaft occupies lower half
            byte in_shaft =
                (y < half_height) &&
                (x >= center - shaft_width / 2) &&
                (x <= center + shaft_width / 2);
            data[index++] = (in_head || in_shaft) ? fill : empty;
        }
    }
    // Pass 2: generate outline
    index = 0;
    int pixel_count = size.x * size.y;
    color* source = malloc(sizeof(color) * pixel_count);
    memcpy(source, data, sizeof(color) * pixel_count);
    for (int y = 0; y < size.y; y++) {
        for (int x = 0; x < size.x; x++, index++) {
            if (!color_equal(source[index], fill))  {
                continue;
            }
            byte border = 0;
            for (int oy = -(int)thickness; oy <= (int)thickness && !border; oy++) {
                for (int ox = -(int)thickness; ox <= (int)thickness; ox++) {
                    int nx = x + ox;
                    int ny = y + oy;
                    if (nx < 0 || ny < 0 ||
                        nx >= size.x || ny >= size.y) {
                        border = 1;
                        break;
                    }
                    int nindex = ny * size.x + nx;
                    if (!color_equal(source[nindex], fill)) {
                        border = 1;
                        break;
                    }
                }
            }
            if (border) {
                data[index] = outline;
            }
        }
    }
    free(source);
}

zox_sys2(ArrowTextureSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_in(TextureSize);
    zox_sys_in(FillColor);
    zox_sys_in(OutlineColor);
    zox_sys_in(OutlineThickness);
    zox_sys_out(TextureData);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Generate, trigger);
        zox_sys_i(TextureSize, size);
        zox_sys_i(FillColor, fill);
        zox_sys_i(OutlineColor, outline);
        zox_sys_i(OutlineThickness, thickness);
        zox_sys_o(TextureData, data);
        if (trigger->value != zox_dirty_active) {
            continue;
        }
        resize_TextureData(data, size->value.x * size->value.y);
        generate_texture_arrow(
            data->value,
            size->value,
            fill->value,
            outline->value,
            thickness->value);
        zox_add(e, TextureDirty);
        if (dbg_log) {
            zox_log("Arrow Texture Generated [%s] fill [%ix%ix%ix%i] outline [%ix%ix%ix%i] Size [%ix%i] Thickness [%i]", zox_get_name(e), fill->value.r, fill->value.g, fill->value.b, fill->value.a, outline->value.r, outline->value.g, outline->value.b, outline->value.a, size->value.x, size->value.y, thickness->value);
        }
    }
} zox_sys_end(ArrowTextureSystem);
