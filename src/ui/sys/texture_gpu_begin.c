zox_sys2(TextureGpuBeginSystem) {
    zox_sys_begin();
    zox_sys_in(InitializeEntity);
    zox_sys_out(TextureGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(InitializeEntity, initialize);
        zox_sys_o(TextureGPULink, textureGPULink);
        if (initialize->value != zox_dirty_active) {
            continue;
        }
        textureGPULink->value = spawn_gpu_texture_buffer();
    }
} zox_sys_end(TextureGpuBeginSystem);
