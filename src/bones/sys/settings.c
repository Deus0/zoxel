// NOTE: Add bone settings to our App!
zox_sys2(BonesSettingsSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LoadSettings);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(LoadSettings, load);
        if (load->value != zox_load_settings_spawn) {
            continue;
        }
#ifdef zox_debug_settings
        spawn_setting_byte(world, e, "Disable Bones", disable_bones);
        spawn_setting_byte(world, e, "Transparent Mesh", transparent_meshes);
        spawn_setting_byte(world, e, "Bone Gizmos", render_bones);
        spawn_setting_byte(world, e, "Paint Weights", paint_bone_weights);
#endif
    }
} zox_sys_end(BonesSettingsSystem);

zox_sys2(BonesSettingsDirtySystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SettingDirty);
    zox_sys_in(ZoxName);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SettingDirty, state);
        zox_sys_i(ZoxName, name);
        if (state->value != zox_dirty_active) {
            continue;
        }
        if (zox_has(e, SettingByte)) {
            byte value = zox_getv(e, SettingByte);
            if (dbg_log) {
                zox_log("Byte Setting [%s] Set [%i]", name->value, value);
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
} zox_sys_end(BonesSettingsDirtySystem);
