zoxc_guint(UvsGPULink);

ECS_DTOR(UvsGPULink, ptr, {
    zox_gpu_dispose_buffer(ptr->value);
})

void on_destroyed_UvsGPULink(iter *it) {
    byte dbg_log = 0;
    zox_sys_begin();
    zox_sys_out(UvsGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(UvsGPULink, component);
        if (!component->value) {
            continue;
        }
        if (dbg_log) {
            zox_log("UvsGPULink Destroy [%i]", component->value);
        }
        zox_gpu_dispose_buffer(component->value);
        component->value = 0;
    }
}
