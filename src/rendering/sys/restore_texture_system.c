zox_sys2(TextureRestoreSystem) {
    zox_sys_begin();
    zox_sys_out(TextureGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(TextureGPULink, texture);
        texture->value = spawn_gpu_texture_buffer();
    }
} zox_sys_end(TextureRestoreSystem);

zox_sys2(TextureDirtyRestoreSystem) {
    zox_sys_begin();
    zox_sys_out(TextureDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(TextureDirty, dirty);
        dirty->value = 1;
    }
} zox_sys_end(TextureDirtyRestoreSystem);