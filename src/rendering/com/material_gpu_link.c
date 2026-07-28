zoxc_guint(MaterialGPULink);
zoxc_guint(MaterialInstancedGPULink);

ECS_DTOR(MaterialGPULink, ptr, {
    zox_dispose_material(ptr->value);
})

void on_destroyed_MaterialGPULink(iter *it) {
    byte dbg_log = 0;
    zox_sys_begin();
    zox_sys_out(MaterialGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(MaterialGPULink, component);
        if (!component->value) {
            continue;
        }
        if (dbg_log) {
            zox_log("MaterialGPULink Destroy [%i]", component->value);
        }
        zox_dispose_material(component->value);
        component->value = 0;
    }
}