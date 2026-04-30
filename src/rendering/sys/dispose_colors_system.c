void MeshColorsGPUDisposeSystem(iter *it) {
    zox_sys_begin()
    zox_sys_in(ColorsGPULink)
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(ColorsGPULink, colors);
        zox_gpu_dispose_buffer(colors->value);
    }
} zoxd_system(MeshColorsGPUDisposeSystem)
