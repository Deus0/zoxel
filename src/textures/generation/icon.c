/*void generate_texture_icon(
    color *data,
    const int2 size,
    const color fill_color,
    const color outline_color,
    const byte frame_thickness,
    const byte radius,
    const byte is_noise
) {
    int index = 0;
    const byte is_odd = radius % 2 != 0;
    int2 mid_positeion = int2_half(size);
    int2 pos = { 0, 0 };
    for (pos.y = 0; pos.y < size.y; pos.y++) {
        for (pos.x = 0; pos.x < size.x; pos.x++) {
            // even numbers fix for radiuse
            int2 check_position = pos;
            if (is_odd) {
                if (check_position.x <= radius) {
                    check_position.x += 1;
                }
                if (check_position.y <= radius) {
                    check_position.y += 1;
                }
            }
            const int distance_to_mid = int2_distance(check_position, mid_position);
            if (distance_to_mid < radius) {
                data[index] = fill_color;
            } else if (distance_to_mid == radius) {
                data[index] = outline_color;
            } else {
                data[index] = empty_color;
            }
            index++;
        }
    }
}*/


// new

void generate_texture_icon(
    color *data,
    const int2 size,
    const color fill_color,
    const color outline_color,
    const byte frame_thickness,
    const byte radius,
    const byte is_noise
) {
    int index = 0;
    const byte is_odd = radius % 2 != 0;
    const int2 mid_position = int2_half(size);
    int2 pos;

    for (pos.y = 0; pos.y < size.y; pos.y++) {
        for (pos.x = 0; pos.x < size.x; pos.x++) {

            // Fix for odd radius
            int2 check_position = pos;
            if (is_odd) {
                if (check_position.x <= radius) check_position.x++;
                if (check_position.y <= radius) check_position.y++;
            }

            const int distance_to_mid = int2_distance(check_position, mid_position);

            // Choose base pixel
            color pixel = empty_color;
            if (distance_to_mid <= radius) {
                if (distance_to_mid >= radius - frame_thickness) {
                    pixel = outline_color; // Frame/outline
                } else {
                    pixel = fill_color; // Fill inside
                }
            }

            // Apply noise if enabled
            if (is_noise) {
                unsigned int noise_val = (pos.x * 73856093u) ^ (pos.y * 19349663u);
                noise_val = (noise_val >> 3) & 0xFF;
                if (noise_val < 20) { // ~8% noise
                    pixel = (color) {
                        .r = pixel.r ^ 0x55,
                        .g = pixel.g ^ 0x55,
                        .b = pixel.b ^ 0x55,
                        .a = pixel.a
                    };
                }
            }

            data[index++] = pixel;
        }
    }
}

