zox_sys2(ScreenRenderTextureSystem) {
    byte dbg_log = 0;
    zox_sys_begin();
    zox_sys_in(LayoutSize);
    zox_sys_out(TextureSize);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(LayoutSize, lsize);
        zox_sys_o(TextureSize, tsize);
        tsize->value = scale_viewport(lsize->value);
        if (dbg_log) {
            zox_log("Render Texture Updated", tsize->value.x, tsize->value.y);
        }
    }
} zox_sys_end(ScreenRenderTextureSystem);
