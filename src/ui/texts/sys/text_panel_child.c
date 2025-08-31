void ZextParentBackgroundSystem(iter *it) {

    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TextDirty);
    zox_sys_in(TextData);
    zox_sys_in(TextSize);
    zox_sys_in(TextPadding);
    // zox_sys_in(MeshAlignment);
    zox_sys_in(ParentLink);

    for (int i = 0; i < it->count; i++) {

        zox_sys_i(TextDirty, dirty);
        zox_sys_i(ParentLink, parent);
        zox_sys_i(TextData, text);
        zox_sys_i(TextSize, size);
        zox_sys_i(TextPadding, textPadding);
        // zox_sys_i(MeshAlignment, meshAlignment);

        if (dirty->value != zox_dirty_active) {
            continue;
        }
        if (!zox_valid(parent->value) || !zox_has(parent->value, ZextLabel)) {
            continue;
        }
        /*zox_geter_value(parent->value, GenerateTexture, byte, generate_texture);
        if (generate_texture) {
            return;
        }*/

        zox_geter_value(parent->value, LayoutSize, int2, old_size);
        const int2 new_size = calculate_zext_size(
            text->value,
            text->length,
            size->value,
            textPadding->value,
            default_line_padding
        );

        if (!int2_equals(old_size, new_size)) {
            zox_set(parent->value, LayoutSize, { new_size });
            zox_set(parent->value, LayoutSizeDirty, { zox_dirty_trigger });
        }
    }
} zoxd_system2(ZextParentBackgroundSystem);