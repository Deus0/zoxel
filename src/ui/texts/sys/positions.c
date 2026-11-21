int2 calculate_position(const byte *data, int length, int data_index, byte font_size, byte text_alignment, byte2 padding, byte line_padding) {
    if (data_index >= length) {
        zox_loge("DataIndex greater than length [%i] > [%i]", data_index, length);
        return int2_zero;
    }
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
    if (text_alignment == zox_alignment_centre) {
        return position;
    }
    // default was centred
    if (text_alignment == zox_alignment_right) {
        position.x -= size.x / 2;
        //position.x -= padding.x;
    } else if (text_alignment == zox_alignment_left) {
        position.x += size.x / 2;
        //position.x += padding.x;
    } else if (text_alignment == zox_alignment_top_right) {
        position.x -= size.x / 2;
        position.y -= size.y / 2;
    } else if (text_alignment == zox_alignment_bottom_right) {
        position.x -= size.x / 2;
        position.y += size.y / 2;
    } else if (text_alignment == zox_alignment_top_left) {
        position.x += size.x / 2;
        position.y -= size.y / 2;
    } else if (text_alignment == zox_alignment_bottom_left) {
        position.x += size.x / 2;
        position.y += size.y / 2;
    }
    // zox_log(" i[%i] %c - xy [%ix%i] - zig pos %ix%i\n", data_index, convert_to_ascii(textData->value[data_index]), x, y, position.x, position.y)
    return position;
}

// Centralized position setting for text zigels
// Centralized position setting for text zigels
void texts_position_system(iter* it) {
    zox_sys_on_begin();
    byte is_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TextData);
    zox_sys_in(TextFontSize);
    zox_sys_in(TextAlignment);
    zox_sys_in(TextPadding);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TextData, text_data);
        zox_sys_i(TextFontSize, size);
        zox_sys_i(TextAlignment, alignment);
        zox_sys_i(TextPadding, padding);
        /*if (dirty->value != zox_dirty_end) {
            continue;
        }*/
        iter it2 = zox_children(world, e);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                entity e2 = it2.entities[j];
                if (!zox_valid(e2) || !zox_has(e2, ChildIndex)) {
                    continue;
                }
                // NOTE: glyphs aren't iterated in order, so use their index
                const uint child_index = zox_getv(e2, ChildIndex);
                if (!zox_has(e2, LayoutPosition)) {
                    zox_logw("Glyph [%s] is missing Layout Components",
                        zox_get_name(e2));
                    continue;
                }
                uint index = child_index_to_text_array_index(text_data->value, text_data->length, child_index);
                int2 position = calculate_position(text_data->value, text_data->length, index, size->value, alignment->value, padding->value, default_line_padding);
                zox_mut_begin(e2, LayoutPosition, lposition);
                if (!int2_equals(lposition->value, position)) {
                    lposition->value = position;
                    if (is_log) {
                        zox_log("Positioned [%i]  Glyph [%s]:[%i] at [%ix%i]", child_index, zox_getn(e2), j, position.x, position.y);
                    }
                }
            }
        }
    }
    zox_sys_on_end();
} zoxd_system(texts_position_system);
