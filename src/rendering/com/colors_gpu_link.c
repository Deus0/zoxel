zoxc_guint(ColorsGPULink);

ECS_DTOR(ColorsGPULink, ptr, {
    zox_gpu_dispose_buffer(ptr->value);
})

void on_destroyed_ColorsGPULink(iter *it) {
    byte dbg_log = 0;
    zox_sys_begin();
    zox_sys_out(ColorsGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(ColorsGPULink, component);
        if (!component->value) {
            continue;
        }
        if (dbg_log) {
            zox_log("ColorsGPULink Destroy [%i]", component->value);
        }
        zox_gpu_dispose_buffer(component->value);
        component->value = 0;
    }
}
