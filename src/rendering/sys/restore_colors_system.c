zox_sys2(ColorsGPULinkRestoreSystem) {
    zox_sys_begin()
    zox_sys_out(ColorsGPULink)
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(ColorsGPULink, colorsGPULink)
        colorsGPULink->value = zox_gpu_create_buffer();
    }
} zox_sys_end(ColorsGPULinkRestoreSystem);
