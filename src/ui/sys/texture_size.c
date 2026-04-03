zox_sys2(TextureSizeSystem) {
    zox_sys_begin();
    zox_sys_in(LayoutSizeDirty);
    zox_sys_in(LayoutSize);
    zox_sys_out(TextureSize);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(LayoutSizeDirty, dirty);
        zox_sys_i(LayoutSize, layout_size);
        zox_sys_o(TextureSize, tsize);

        if (dirty->value == zox_dirty_active) {
            //if (tsize->value.x == 0 || tsize->value.y == 0) {
            tsize->value = layout_size->value;

            /*zox_sys_world();
            zox_sys_e();
            zox_log("Set texture size to layout size [%s] [%ix%i]", zox_get_name(e), tsize->value.x, tsize->value.y);*/
            //}
        }
    }
} zox_sys_end(TextureSizeSystem);
