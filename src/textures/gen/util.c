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

void generate_texture_fill(color* data, int2 size, color fill_color) {
    int2 position;
    for (position.x = 0; position.x < size.x; position.x++) {
        for (position.y = 0; position.y < size.y; position.y++) {
            data[int2_array_index(position, size)] = fill_color;
        }
    }
}
