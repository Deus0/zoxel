// calculates the child index, takes out ascii like new line that have no zigel spawns
int calculate_zigel_data_index(const byte *data, int length, int spawn_index) {

    int j = 0;
    for (int i = 0; i < length; i++) {
        if (data[i] != zox_char_newline) {
            if (j == spawn_index) {
                return i;
            }
            j++;
        }
    }

    zox_log_error("calculate_zigel_data_index: j [%i] spawn_index [%i] length [%i]", j, spawn_index, length);

    return 0;
}

int2 calculate_position(const byte *data, int length, int data_index, byte font_size, byte text_alignment, byte2 padding, byte line_padding) {

    int x = get_zext_x(data, data_index);
    int y = get_zext_y(data, length, data_index);
    int2 size = calculate_zext_size(data, length, font_size, padding, line_padding);

    int2 position = int2_zero;
    // add half zigel size offset
    position.x += font_size / 2;
    position.y -= font_size / 2;
    // displace by total text size
    position.x -= (size.x - padding.x * 2) / 2;
    position.y += (size.y - padding.y * 2) / 2;
    // now displace by grid position
    position.x += x * font_size;
    position.y -= y * (font_size + line_padding);
    // position.y += size.y / 2;
    if (text_alignment == zox_mesh_alignment_centred) return position;
    // default was centred
    if (text_alignment == zox_mesh_alignment_right) {
        position.x -= size.x / 2;
        //position.x -= padding.x;
    } else if (text_alignment == zox_mesh_alignment_left) {
        position.x += size.x / 2;
        //position.x += padding.x;
    } else if (text_alignment == zox_mesh_alignment_top_right) {
        position.x -= size.x / 2;
        position.y -= size.y / 2;
    } else if (text_alignment == zox_mesh_alignment_bottom_right) {
        position.x -= size.x / 2;
        position.y += size.y / 2;
    } else if (text_alignment == zox_mesh_alignment_top_left) {
        position.x += size.x / 2;
        position.y -= size.y / 2;
    } else if (text_alignment == zox_mesh_alignment_bottom_left) {
        position.x += size.x / 2;
        position.y += size.y / 2;
    }
    // zox_log(" i[%i] %c - xy [%ix%i] - zig pos %ix%i\n", data_index, convert_to_ascii(textData->value[data_index]), x, y, position.x, position.y)
    return position;
}

// Centralized position setting for text zigels
zox_sys2(ZigelPositionSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TextDirty);  // TextSizeDirty
    zox_sys_in(Children);
    zox_sys_in(TextData);
    zox_sys_in(TextFontSize);
    zox_sys_in(TextAlignment);
    zox_sys_in(TextPadding);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(TextDirty, dirty);
        zox_sys_i(Children, children);
        zox_sys_i(TextData, text_data);
        zox_sys_i(TextFontSize, size);
        zox_sys_i(TextAlignment, alignment);
        zox_sys_i(TextPadding, padding);

        if (dirty->value != zox_dirty_end) {
            continue;
        }

        for (int j = 0; j < children->length; j++) {
            entity e2 = children->value[j];

            if (!zox_valid(e2)) {
                continue;
            }

            int data_index = calculate_zigel_data_index(text_data->value, text_data->length, j);

            int2 position = calculate_position(text_data->value, text_data->length, data_index, size->value, alignment->value, padding->value, default_line_padding);

            zox_set(e2, LayoutPosition, { position });
            zox_set(e2, LayoutPositionDirty, { zox_dirty_trigger });
        }
    }
} zox_sys_end(ZigelPositionSystem);
