void TextureSizeGenerateSystem(iter *it) {
    zox_sys_begin();
    zox_sys_in(LayoutSizeDirty);
    zox_sys_out(GenerateTexture);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(LayoutSizeDirty, dirty);
        zox_sys_o(GenerateTexture, generate);
        if (dirty->value == zox_dirty_active) {
            generate->value = zox_dirty_trigger;
        }
    }
} zoxd_system2(TextureSizeGenerateSystem);