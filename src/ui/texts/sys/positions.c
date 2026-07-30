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
/*zox_sys2(ZigelPositionSystem) {
    byte is_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ChildIndex);
    zox_sys_out(ZigelDirty);
    zox_sys_out(LayoutPosition);
    zox_sys_out(LayoutPositionDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ChildIndex, child_index);
        zox_sys_o(ZigelDirty, zigel_dirty);
        zox_sys_o(LayoutPosition, position);
        zox_sys_o(LayoutPositionDirty, position_dirty);
        if (zigel_dirty->value != zox_zigel_dirty_position) {
            continue;
        }
        entity parent = zox_get_parent(world, e);
#ifdef zox_safety_checks
        if (!zox_valid(parent)) {
            zox_loge("Zigel has no parent [%s]", zox_get_name(e));
            continue;
        }
#endif
        byte text_dirty = zox_getv(parent, TextDirty);
        if (text_dirty != zox_dirty_end) {
            continue;
        }
        const TextData* text_data = zox_get(parent, TextData);
        byte font_size = zox_getv(parent, TextFontSize);
        byte alignment = zox_getv(parent, TextAlignment);
        byte2 padding = zox_getv(parent, TextPadding);
        uint array_index = child_index_to_text_array_index(text_data->value, text_data->length, child_index->value);
        if (array_index >= text_data->length) {
            zox_loge("(ZigelPositionSystem) [%s]: [%s] Has a array_index oob [%i] >= [%i] (child_index_to_text_array_index)", zox_getn(parent), zox_getn(e), array_index, text_data->length);
            continue;
        }
        int2 new_position = calculate_position(text_data->value, text_data->length, array_index, font_size, alignment, padding, default_line_padding);
        if (!int2_equals(position->value, new_position))
        {
            position->value = new_position;
            position_dirty->value = zox_dirty_trigger;
            if (is_log) {
                zox_log("Positioned Zigel: [%s]:[%i] at [%ix%i]", zox_get_name(e), child_index->value, new_position.x, new_position.y);
            }
        }
        zigel_dirty->value = zox_zigel_dirty_update;
    }
} zox_sys_end(ZigelPositionSystem);*/

// Centralized position setting for text zigels
zox_sys2(TextsPositionSystem) {
    byte is_log = 1;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TextDirty);
    zox_sys_in(TextData);
    zox_sys_in(TextFontSize);
    zox_sys_in(TextAlignment);
    zox_sys_in(TextPadding);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TextDirty, dirty);
        zox_sys_i(TextData, text_data);
        zox_sys_i(TextFontSize, size);
        zox_sys_i(TextAlignment, alignment);
        zox_sys_i(TextPadding, padding);
        if (dirty->value != zox_dirty_end) {
            continue;
        }
        uint child_index = 0;
        iter it2 = zox_children(world, e);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                entity e2 = it2.entities[j];
                if (!zox_valid(e2)) {
                    continue;
                }
                if (!zox_has(e2, LayoutPosition) || !zox_has(e2, LayoutPositionDirty)) {
                    zox_logw("Zigel [%s] is missing Layout Component/s", zox_get_name(e2));
                    continue;
                }
                uint index = child_index_to_text_array_index(text_data->value, text_data->length, child_index);
                int2 position = calculate_position(text_data->value, text_data->length, index, size->value, alignment->value, padding->value, default_line_padding);
                zox_mut_begin(e2, LayoutPosition, lposition);
                zox_mut_begin(e2, LayoutPositionDirty, ldirty);
                if (!int2_equals(lposition->value, position))
                {
                    lposition->value = position;
                    ldirty->value = zox_dirty_trigger;
                    if (is_log) {
                        zox_log("Positioned [%i]  Zigel [%s]:[%i] at [%ix%i]", child_index, zox_getn(e2), j, position.x, position.y);
                    }
                }
                child_index++;
            }
        }
    }
} zox_sys_end(TextsPositionSystem);
