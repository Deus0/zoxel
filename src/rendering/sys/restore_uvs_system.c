zox_sys2(UvsGPULinkRestoreSystem) {
    zox_sys_begin()
    zox_sys_out(UvsGPULink)
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(UvsGPULink, uvsGPULink)
        uvsGPULink->value = zox_gpu_create_buffer();
    }
} zox_sys_end(UvsGPULinkRestoreSystem);
