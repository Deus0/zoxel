void add_texture_outline(
    color* pixels,
    const int2 size,
    const color air_color,
    const color outline_color
) {
    if (!size.x || !size.y) {
        return;
    }

    const int pixel_count = size.x * size.y;

    byte* outline_mask = calloc(pixel_count, sizeof(byte));
    if (!outline_mask) {
        return;
    }

    const int offsets[8][2] = {
        {-1,-1}, {0,-1}, {1,-1},
        {-1, 0},         {1, 0},
        {-1, 1}, {0, 1}, {1, 1}
    };

    // Detect outline locations
    for (int y = 0; y < size.y; y++) {
        for (int x = 0; x < size.x; x++) {

            const int index =
            int2_array_index((int2){x, y}, size);

            if (!color_equal(pixels[index], air_color)) {
                continue;
            }

            for (int i = 0; i < 8; i++) {
                const int nx = x + offsets[i][0];
                const int ny = y + offsets[i][1];

                if (nx < 0 || nx >= size.x ||
                    ny < 0 || ny >= size.y) {
                    continue;
                    }

                    const int neighbor =
                    int2_array_index((int2){nx, ny}, size);

                if (!color_equal(pixels[neighbor], air_color)) {
                    outline_mask[index] = 1;
                    break;
                }
            }
        }
    }

    // Apply outline
    for (int i = 0; i < pixel_count; i++) {
        if (outline_mask[i]) {
            pixels[i] = outline_color;
        }
    }

    free(outline_mask);
}
