void MeshGPUDisposeSystem(iter *it) {
    zox_sys_begin()
    zox_sys_in(MeshGPULink)
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(MeshGPULink, meshGPULink);

        zox_gpu_dispose_buffer(meshGPULink->value.x);
        zox_gpu_dispose_buffer(meshGPULink->value.y);
    }
} zoxd_system(MeshGPUDisposeSystem)
