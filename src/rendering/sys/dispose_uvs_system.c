zox_sys2(MeshUvsGPUDisposeSystem) {
    zox_sys_begin()
    zox_sys_in(UvsGPULink)
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(UvsGPULink, uvsGPULink)
        zox_gpu_dispose_buffer(uvsGPULink->value);
    }
} zox_sys_end(MeshUvsGPUDisposeSystem);
