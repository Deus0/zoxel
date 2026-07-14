void generate_texture_frame(color* data, int2 size, color fill_color, color outline_color, byte frame_thickness, byte corner_size, byte is_noise) {
    int index = 0;
    int2 pixel_position = int2_zero;
    for (pixel_position.y = 0; pixel_position.y < size.y; pixel_position.y++) {
        for (pixel_position.x = 0; pixel_position.x < size.x; pixel_position.x++) {
            int distance_to_corner_a = pixel_position.x + pixel_position.y;
            int distance_to_corner_b = (size.x - 1 - pixel_position.x) + pixel_position.y;
            int distance_to_corner_c = (size.x - 1 - pixel_position.x) + (size.y - 1 - pixel_position.y);
            int distance_to_corner_d = pixel_position.x + (size.y - 1 - pixel_position.y);
            if (distance_to_corner_a < corner_size || distance_to_corner_b < corner_size || distance_to_corner_c < corner_size || distance_to_corner_d < corner_size) {
                data[index] = empty_color;
            } else {
                data[index] = fill_color;
            }
            index++;
        }
    }
    // outline of frame
    index = 0;
    for (pixel_position.y = 0; pixel_position.y < size.y; pixel_position.y++) {
        for (pixel_position.x = 0; pixel_position.x < size.x; pixel_position.x++) {
            if (!color_equal(data[index], fill_color)) {
                index++;
                continue;
            }
            if (pixel_position.x < frame_thickness || pixel_position.y < frame_thickness || pixel_position.x > size.x - 1 - frame_thickness || pixel_position.y > size.y - 1 - frame_thickness) {
                data[index] = outline_color;
            }
            // only floodfill check corners of the texture to save processing time
            else if ((pixel_position.x < corner_size && pixel_position.y < corner_size) ||
                (pixel_position.x > size.x - corner_size && pixel_position.y < corner_size) ||
                (pixel_position.x > size.x - corner_size && pixel_position.y > size.y - corner_size) ||
                (pixel_position.x < corner_size && pixel_position.y > size.y - corner_size)) {
                if (check_texture(data, size, pixel_position, empty_color, frame_thickness)) {
                    data[index] = outline_color;
                }
            }
            index++;
        }
    }
    // add noise to fill parts
    if (is_noise) {
        index = 0;
        const int fill_noise_addition = 55;
        const int outline_noise_addition = 25;
        for (pixel_position.y = 0; pixel_position.y < size.y; pixel_position.y++) {
            for (pixel_position.x = 0; pixel_position.x < size.x; pixel_position.x++) {
                if (color_equal(data[index], fill_color)) {
                    data[index].r += rand() % fill_noise_addition;
                    data[index].g += rand() % fill_noise_addition;
                    data[index].b += rand() % fill_noise_addition;
                } else if (color_equal(data[index], outline_color)) {
                    data[index].r += rand() % outline_noise_addition;
                    data[index].g += rand() % outline_noise_addition;
                    data[index].b += rand() % outline_noise_addition;
                }
                index++;
            }
        }
    }
}

zox_sys2(FrameTextureSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TextureSize);
    zox_sys_in(FillColor);
    zox_sys_in(OutlineThickness);
    zox_sys_in(FrameCorner);
    zox_sys_out(GenerateTexture);
    zox_sys_out(TextureData);
    zox_sys_out(TextureDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TextureSize, size);
        zox_sys_i(FillColor, fill);
        zox_sys_i(OutlineThickness, thickness);
        zox_sys_i(FrameCorner, edge);
        zox_sys_o(TextureData, data);
        zox_sys_o(TextureDirty, dirty);
        zox_sys_o(GenerateTexture, generate);
        if (generate->value != zox_generate_texture_run) {
            continue;
        }
        resize_TextureData(data, size->value.x * size->value.y);
        byte add_noise = zox_has(e, TextureAddNoise);
        color fill_color = fill->value;
        color outline_color;
        if (zox_has(e, OutlineColor)) {
            outline_color = zox_getv(e, OutlineColor);
        } else {
            outline_color = fill_color;
        }
        generate_texture_frame(data->value, size->value, fill_color, outline_color, thickness->value, edge->value, add_noise);
        dirty->value = zox_dirty_trigger;
        generate->value = zox_generate_texture_end;
        // voronoi2D(textureData->value, textureSize->value, color_gray, color_gray_dark, 0.7f);
        if (dbg_log) {
            zox_log("Frame Texture generated [%s] at [%f] fill [%ix%ix%ix%i]", zox_get_name(e), zox_current_time, fill->value.r, fill->value.g, fill->value.b, fill->value.a);
        }
    }
} zox_sys_end(FrameTextureSystem);
