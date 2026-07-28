zoxc_guint(UboGPULink);

// actually this gets called during flecs table changes
ECS_DTOR(UboGPULink, ptr, {
    // if (ptr->value != 0) glDeleteBuffers(1, &ptr->value);
})

void on_destroyed_UboGPULink(iter *it) {
    byte dbg_log = 0;
    zox_sys_begin();
    zox_sys_out(UboGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(UboGPULink, component);
        if (!component->value) {
            continue;
        }
        if (dbg_log) {
            zox_log("UboGPULink Destroy [%i]", component->value);
        }
        zox_gpu_dispose_buffer(component->value);
        component->value = 0;
    }
}