int get_zext_max_x(
    const byte *data,
    const int length
) {
    int x = 0;
    int max_x = 0;
    for (int i = 0; i < length; i++) {
        if (data[i] == zox_char_newline) {
            x = 0;
        } else {
            x++;
            if (x > max_x) max_x = x;
        }
    }
    if (x > max_x) {
        max_x = x;
    }
    return max_x;
}

int get_zext_max_y(
    const byte *data,
    const int length
) {
    int y = 1;
    for (int i = 0; i < length; i++) {
        if (data[i] == zox_char_newline && i != length - 1) y++;
    }
    return y;
}

int get_zext_x(
    const byte *data,
    const int data_index
) {
    int x = 0;
    for (int i = 0; i < data_index; i++) {
        if (data[i] == zox_char_newline) x = 0;
        else x++;
    }
    return x;
}

int get_zext_y(
    const byte *data,
    const int length,
    const int data_index
) {
    int y = 0;
    for (int i = 0; i < data_index; i++) {
        if (data[i] == zox_char_newline && i != length - 1) y++;
    }
    return y;
}

int calculate_total_zigels(
    const byte *data,
    const int length
) {
    int count = 0;
    for (int i = 0; i < length; i++) {
        if (data[i] != zox_char_newline) count++;
    }
    return count;
}

// from a zigel (child?) index, get the data char code
byte calculate_zigel_index(
    const byte *data,
    const int length,
    const int spawn_index
) {
    int j = 0;
    for (int i = 0; i < length; i++) {
        if (data[i] != zox_char_newline) {
            if (j == spawn_index) return data[i];
            j++;
        }
    }
    return 0;
}

int2 calculate_zext_size(
    const byte *data,
    const int length,
    const byte font_size,
    const byte2 padding,
    const byte line_padding
) {
    int x = get_zext_max_x(data, length);
    int y = get_zext_max_y(data, length);
    // zox_log("sizey: %i\n", y)
    return (int2) { font_size * x + padding.x * 2, (font_size) * y + (y - 1) * line_padding + padding.y * 2 };
}

// a wrapper for spawn_zigel, also sets position for text character
entity spawn_zext_zigel(
    ecs* world,
    const TextData* text_data,
    SpawnZigel *data
) {
    /*data->element.position = calculate_zigel_position(
        text_data->value,
        text_data->length,
        data->zigel.data_index,
        data->element.size.x,
        data->zext.text_alignment,
        data->zext.text_padding,
        default_line_padding
    );*/
    data->element.anchor = float2_half;
    return spawn_zigel(world, data);
}
