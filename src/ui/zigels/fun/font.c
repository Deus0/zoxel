void clear_texture(color* data, int2 size, color clear) {
    int index = 0;
    for (int k = 0; k < size.y; k++) {
        for (int j = 0; j < size.x; j++) {
            data[index] = clear;
            index++;
        }
    }
}

void generate_font_texture(
    color* data,
    int2 size,
    const byte2* points,
    int points_length,
    color line_color,
    color fill_color,
    byte is_shapes,
    byte fill_thickness,
    byte outline_thickness,
    float2 point_padding,
    color clear_color
) {
    if (!points_length) {
        return;
    }
    if (is_shapes) {
        generate_font_lines(data, size, points, points_length, line_color, point_padding);
        scanline_fill_texture(data, size, clear_color, line_color, fill_color);
        if (outline_thickness) {
            generate_splotches_lines(
                data,
                size,
                points, points_length,
                line_color,
                outline_thickness,
                point_padding
            );
        }
    } else {
        fill_thickness++;
        generate_splotches_lines(
            data,
            size,
            points, points_length,
            line_color,
            fill_thickness,
            point_padding
        );
    }
}
