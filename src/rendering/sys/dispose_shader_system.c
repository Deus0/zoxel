zox_sys2(ShaderDisposeSystem) {
    zox_sys_begin()
    zox_sys_in(ShaderGPULink)
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(ShaderGPULink, shaderGPULink)
#ifdef zox_log_gpu_management
        zox_log(" + disposing [%s] [%ix%i]\n", zox_get_name(it->entities[i]), shaderGPULink->value.x, shaderGPULink->value.x)
#endif
        zox_gpu_dispose_shader(shaderGPULink->value.x);
        zox_gpu_dispose_shader(shaderGPULink->value.y);
    }
} zox_sys_end(ShaderDisposeSystem);
