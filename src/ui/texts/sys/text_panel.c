// this needs textData with Mesh in one entity
zox_sys2(TextBackgroundSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TextData);
    zox_sys_in(TextFontSize);
    zox_sys_in(TextPadding);
    zox_sys_out(LayoutSize);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TextData, text);
        zox_sys_i(TextFontSize, size);
        zox_sys_i(TextPadding, padding);
        zox_sys_o(LayoutSize, layout_size);
        int2 new_size = calculate_zext_size(
            text->value,
            text->length,
            size->value,
            padding->value,
            default_line_padding);
        layout_size->value = new_size;
        zox_add(e, LayoutSizeDirty);
    }
} zox_sys_end(TextBackgroundSystem);
