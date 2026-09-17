zox_sys2(TextParentBackgroundSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TextData);
    zox_sys_in(TextFontSize);
    zox_sys_in(TextPadding);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TextData, text);
        zox_sys_i(TextFontSize, size);
        zox_sys_i(TextPadding, textPadding);
        /*if (dirty->value != zox_dirty_active) {
            continue;
        }*/
        entity parent = zox_get_parent(world, e);
        if (!zox_valid(parent) ||
            !zox_has(parent, LayoutSize) ||
            !zox_has(parent, TextResizer))
        {
            continue;
        }
        zox_geter_value(parent, LayoutSize, int2, old_size);
        int2 new_size = calculate_zext_size(
            text->value,
            text->length,
            size->value,
            textPadding->value,
            default_line_padding);
        if (!int2_equals(old_size, new_size)) {
            zox_setv(parent, LayoutSize, new_size);
            zox_setv(parent, LayoutSizeDirty, zox_dirty_trigger);
        }
    }
} zox_sys_end(TextParentBackgroundSystem);
