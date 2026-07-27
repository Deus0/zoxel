zoxc_guint(TextureGPULink);

/*
 * It has no knowledge of whether this is:
    entity deletion
    component removal
    table migration
*/

ECS_DTOR(TextureGPULink, ptr, {
    zox_log("TextureGPULink DTOR [%i]", ptr->value);
    zox_gpu_dispose_texture(ptr->value);
})

void on_destroyed_TextureGPULink(iter *it) {
    byte dbg_log = 0;
    zox_sys_begin();
    zox_sys_out(TextureGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(TextureGPULink, component);
        if (!component->value) {
            continue;
        }
        if (dbg_log) {
            zox_log("TextureGPULink Destroy [%i]", component->value);
        }
        zox_gpu_dispose_texture(component->value);
        component->value = 0;
    }
}
