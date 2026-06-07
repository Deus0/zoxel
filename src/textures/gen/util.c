const color empty_color = { 0, 0, 0, 0 };

byte check_texture(color *data, int2 size, int2 pixel_position, color find_color, int distance) {
    if (!int2_in_bounds(pixel_position, size)) {
        return 0;
    }
    if (color_equal(find_color, data[int2_array_index(pixel_position, size)])) {
        return 1;
    }
    if (distance >= 0) {
        distance--;
        if (check_texture(data, size, int2_down(pixel_position), find_color, distance)) return 1;
        if (check_texture(data, size, int2_up(pixel_position), find_color, distance)) return 1;
        if (check_texture(data, size, int2_left(pixel_position), find_color, distance)) return 1;
        if (check_texture(data, size, int2_right(pixel_position), find_color, distance)) return 1;
    }
    return 0;
}

void generate_texture_frame(color* data, int2 size, color fill_color, color outline_color, byte frame_thickness, byte corner_size, byte is_noise) {
    if (!data) {
        return;
    }
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

void generate_texture_fill(color* data, int2 size, color fill_color) {
    int2 position;
    for (position.x = 0; position.x < size.x; position.x++) {
        for (position.y = 0; position.y < size.y; position.y++) {
            data[int2_array_index(position, size)] = fill_color;
        }
    }
}
