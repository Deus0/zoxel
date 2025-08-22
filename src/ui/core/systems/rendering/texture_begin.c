void TextureGpuBeginSystem(iter *it) {
    if (headless) return;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(InitializeElement);
    zox_sys_out(TextureGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(InitializeElement, initialize);
        zox_sys_o(TextureGPULink, textureGPULink);
        if (initialize->value != zox_dirty_active) {
            continue;
        }
        textureGPULink->value = spawn_gpu_texture_buffer();
    }
} zoxd_system2(TextureGpuBeginSystem);

void TextureDirtyBeginSystem(iter *it) {
    if (headless) return;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(InitializeElement);
    zox_sys_out(TextureDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(InitializeElement, initialize);
        zox_sys_o(TextureDirty, dirty);
        if (initialize->value != zox_dirty_active) {
            continue;
        }
        //if (!zox_has(it->entities[i], GenerateTexture))
        if (dirty->value) dirty->value = zox_dirty_trigger;
    }
} zoxd_system2(TextureDirtyBeginSystem);