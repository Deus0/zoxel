zoxc_guint2(ShaderGPULink);

ECS_DTOR(ShaderGPULink, ptr, {
    zox_gpu_dispose_shader(ptr->value.x);
    zox_gpu_dispose_shader(ptr->value.y);
})

void on_destroyed_ShaderGPULink(iter *it) {
    byte dbg_log = 0;
    zox_sys_begin();
    zox_sys_out(ShaderGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(ShaderGPULink, component);
        if (!component->value.x && !component->value.y) {
            continue;
        }
        if (dbg_log) {
            zox_log("ShaderGPULink Destroy [%ix%i]", component->value.x, component->value.x);
        }
        zox_gpu_dispose_shader(component->value.x);
        zox_gpu_dispose_shader(component->value.y);
        component->value.x = 0;
        component->value.y = 0;
    }
}
