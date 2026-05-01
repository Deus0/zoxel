void MeshGPURestoreSystem(iter *it) {
    zox_sys_begin()
    zox_sys_out(MeshGPULink)
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(MeshGPULink, meshGPULink)
        meshGPULink->value.x = zox_gpu_create_buffer();
        meshGPULink->value.y = zox_gpu_create_buffer();
    }
} zoxd_system(MeshGPURestoreSystem)
