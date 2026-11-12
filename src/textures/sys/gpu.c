// This will be tricky when there is no TextureDirty
void TextureDataRestoreSystem(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_add(e, TextureDirty);
    }
    zox_sys_on_end();
} zoxd_system(TextureDataRestoreSystem);