zox_sys2(TextParentBackgroundSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TextDirty);
    zox_sys_in(TextData);
    zox_sys_in(TextFontSize);
    zox_sys_in(TextPadding);
    zox_sys_in(ParentLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(TextDirty, dirty);
        zox_sys_i(ParentLink, parent);
        zox_sys_i(TextData, text);
        zox_sys_i(TextFontSize, size);
        zox_sys_i(TextPadding, textPadding);

        if (dirty->value != zox_dirty_active) {
            continue;
        }

        if (!zox_valid(parent->value) || !zox_has(parent->value, LayoutSize) || !zox_has(parent->value, TextResizer)) {
            continue;
        }

        zox_geter_value(parent->value, LayoutSize, int2, old_size);
        int2 new_size = calculate_zext_size(text->value, text->length, size->value, textPadding->value, default_line_padding);

        if (!int2_equals(old_size, new_size)) {
            zox_set(parent->value, LayoutSize, { new_size });
            zox_set(parent->value, LayoutSizeDirty, { zox_dirty_trigger });
        }
    }
} zox_sys_end(TextParentBackgroundSystem);
