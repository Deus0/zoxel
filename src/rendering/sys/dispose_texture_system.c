zox_sys2(TextureGPUDisposeSystem) {
    zox_sys_begin();
    zox_sys_in(TextureGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(TextureGPULink, texture);

        if (!texture->value) continue;

        zox_gpu_dispose_texture(texture->value);

        // texture->value = 0;
    }
} zox_sys_end(TextureGPUDisposeSystem);
