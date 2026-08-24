void texture_swap_colors(
    color* data,
    const int2 size,
    const color old_color,
    const color new_color
) {
    for (int i = 0; i < size.x * size.y; i++) {
        if (color_equal(data[i], old_color)) {
            data[i] = new_color;
        }
    }
}