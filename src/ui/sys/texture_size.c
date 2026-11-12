// NOTE: Fixes texture size to layout size, for Fixed pixel resolution entities
void texture_size_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_begin();
    zox_sys_in(LayoutSize);
    zox_sys_out(TextureSize);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(LayoutSize, layout_size);
        zox_sys_o(TextureSize, texture_size);
        texture_size->value = layout_size->value;
    }
    zox_sys_on_end();
} zoxd_system(texture_size_system);
