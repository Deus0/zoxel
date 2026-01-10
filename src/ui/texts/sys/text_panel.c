// this needs textData with Mesh in one entity
zox_sys2(ZextBackgroundSystem) {
    zox_sys_begin();
    zox_sys_in(TextDirty);
    zox_sys_in(TextData);
    zox_sys_in(TextFontSize);
    zox_sys_in(TextPadding);
    zox_sys_out(LayoutSize);
    zox_sys_out(LayoutSizeDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(TextDirty, text_dirty);
        zox_sys_i(TextData, text);
        zox_sys_i(TextFontSize, size);
        zox_sys_i(TextPadding, padding);
        zox_sys_o(LayoutSize, layout_size);
        zox_sys_o(LayoutSizeDirty, layout_size_dirty);

        if (text_dirty->value != zox_dirty_active) {
            continue;
        }

        const int2 new_size = calculate_zext_size(
            text->value,
            text->length,
            size->value,
            padding->value,
            default_line_padding);

        layout_size->value = new_size;
        layout_size_dirty->value = zox_dirty_trigger;

    }
} zox_sys_end(ZextBackgroundSystem);
