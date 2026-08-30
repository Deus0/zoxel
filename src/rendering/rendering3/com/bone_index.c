zoxc_guint(BoneIndexGPULink);

void on_destroyed_BoneIndexGPULink(iter *it) {
    byte dbg_log = 0;
    zox_sys_begin();
    zox_sys_out(BoneIndexGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(BoneIndexGPULink, component);
        if (!component->value) {
            continue;
        }
        if (dbg_log) {
            zox_log("BoneIndexGPULink Destroy [%i]",
                component->value);
        }
        zox_gpu_dispose_buffer(component->value);
        component->value = 0;
    }
}