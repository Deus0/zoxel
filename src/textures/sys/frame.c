static inline byte check_texture(color *data, int2 size, int2 pixel_position, color find_color, int distance) {
    if (!int2_in_bounds(pixel_position, size)) {
        return 0;
    }
    if (color_equal(find_color, data[int2_array_index(pixel_position, size)])) {
        return 1;
    }
    if (distance >= 0) {
        distance--;
        if (check_texture(data, size, int2_down(pixel_position), find_color, distance)) {
            return 1;
        }
        if (check_texture(data, size, int2_up(pixel_position), find_color, distance)) {
            return 1;
        }
        if (check_texture(data, size, int2_left(pixel_position), find_color, distance)) {
            return 1;
        }
        if (check_texture(data, size, int2_right(pixel_position), find_color, distance)) {
            return 1;
        }
    }
    return 0;
}

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

void generate_texture_frame2(color* data, int2 size, color fill_color, color outline_color, byte frame_thickness, byte corner_size, byte is_noise) {
    const int width = size.x;
    const int height = size.y;
    const int fill_noise_addition = 55;
    const int outline_noise_addition = 25;
    int index = 0;
    for (int y = 0; y < height; y++) {
        int by = height - 1 - y;
        for (int x = 0; x < width; x++, index++) {
            int rx = width - 1 - x;
            int d0 = x + y;
            int d1 = rx + y;
            int d2 = rx + by;
            int d3 = x + by;
            int corner_distance = d0;
            if (d1 < corner_distance) corner_distance = d1;
            if (d2 < corner_distance) corner_distance = d2;
            if (d3 < corner_distance) corner_distance = d3;
            color c;
            if (corner_distance < corner_size) {
                // Cut corners
                c = empty_color;
            } else if (x < frame_thickness || y < frame_thickness || x >= width - frame_thickness || y >= height - frame_thickness) {
                // Outer border
                c = outline_color;
            } else if (corner_distance < corner_size + frame_thickness) {
                // Outline around cut corners
                c = outline_color;
            } else {
                c = fill_color;
            }
            if (is_noise) {
                if (color_equal(c, fill_color)) {
                    c.r += rand() % fill_noise_addition;
                    c.g += rand() % fill_noise_addition;
                    c.b += rand() % fill_noise_addition;
                }
                else if (color_equal(c, outline_color)) {
                    c.r += rand() % outline_noise_addition;
                    c.g += rand() % outline_noise_addition;
                    c.b += rand() % outline_noise_addition;
                }
            }
            data[index] = c;
        }
    }
}

zox_sys2(FrameTextureSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TextureSize);
    zox_sys_in(FillColor);
    zox_sys_in(OutlineColor);
    zox_sys_in(OutlineThickness);
    zox_sys_in(FrameCorner);
    zox_sys_out(GenerateTexture);
    zox_sys_out(TextureData);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TextureSize, size);
        zox_sys_i(FillColor, fill);
        zox_sys_i(OutlineColor, outline);
        zox_sys_i(OutlineThickness, thickness);
        zox_sys_i(FrameCorner, edge);
        zox_sys_o(GenerateTexture, generate);
        zox_sys_o(TextureData, data);
        if (generate->value != zox_generate_texture_run) {
            continue;
        }
        uint length = size->value.x * size->value.y;
        if (!length) {
            continue;
        }
        byte add_noise = zox_has(e, TextureAddNoise);
        resize_TextureData(data, length);
        generate_texture_frame2(data->value, size->value, fill->value, outline->value, thickness->value, edge->value, add_noise);
        generate->value = zox_generate_texture_end;
        zox_add(e, TextureDirty);
        zox_sys_increment();
        if (dbg_log) {
            zox_log("Frame Texture generated [%s] fill [%ix%ix%ix%i] Size [%ix%i]", zox_get_name(e), fill->value.r, fill->value.g, fill->value.b, fill->value.a, size->value.x, size->value.y);
        }
    }
    // zox_log("Frame Textures Generated [%i]:[%f]", process_count, calculate_sys_delta());
} zox_sys_end(FrameTextureSystem);
