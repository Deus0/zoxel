void TextureSizeSystem(iter *it) {

    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LayoutSizeDirty);
    zox_sys_in(PixelSize);
    zox_sys_out(TextureSize);

    for (int i = 0; i < it->count; i++) {

        zox_sys_i(LayoutSizeDirty, dirty);
        zox_sys_i(PixelSize, layout_size);
        zox_sys_o(TextureSize, texture_size);

        if (dirty->value == zox_dirty_active) {
            continue;
        }

        texture_size->value = layout_size->value;

    }
} zoxd_system2(TextureSizeSystem);