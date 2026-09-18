void zox_settings_bones(ecs* world, entity app) {
#ifdef zox_debug_settings
        spawn_setting_byte(world, app, "Disable Bones", disable_bones);
        spawn_setting_byte(world, app, "Transparent Mesh", transparent_meshes);
        spawn_setting_byte(world, app, "Bone Gizmos", render_bones);
        spawn_setting_byte(world, app, "Paint Weights", paint_bone_weights);
#endif
}

void zox_settings_dirty_bones(iter* it) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ZoxName);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ZoxName, name);
        if (zox_has(e, SettingByte)) {
            byte value = zox_getv(e, SettingByte);
            if (dbg_log) {
                zox_log("Byte Setting [%s] Set [%i]",
                    name->value,
                    value);
            }
            if (!strcmp(name->value, "Disable Bones")) {
                disable_bones = value;
            } else if (!strcmp(name->value, "Transparent Mesh")) {
                transparent_meshes = value;
            } else if (!strcmp(name->value, "Bones Gizmos")) {
                render_bones = value;
            } else if (!strcmp(name->value, "Paint Weights")) {
                paint_bone_weights = value;
            }
        }
    }
}

