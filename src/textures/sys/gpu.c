// This will be tricky when there is no TextureDirty
zox_sys2(TextureDataRestoreSystem) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_setv(e, TextureDirty, 1);
    }
} zox_sys_end(TextureDataRestoreSystem);