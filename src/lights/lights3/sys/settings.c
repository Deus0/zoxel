zox_sys2(LightsSettingsSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(InitializeEntity);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(InitializeEntity, state);
        if (state->value != zox_dirty_active) {
            continue;
        }
        spawn_setting_byte(world, e, "Disable Lights", disable_terrain_lods);
        // spawn_setting_byte(world, e, "Smooth Lighting", zox_smooth_lighting);
    }
} zox_sys_end(LightsSettingsSystem);

zox_sys2(LightsSettingsDirtySystem) {
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
                zox_log("Float Setting [%s] Set [%f]", name->value, value);
            }
            if (!strcmp(name->value, "Disable Lights")) {
                disable_lights = value;
                if (disable_lights) {
                    zox_disable(zox_id(SmoothLightsBuildSystem));
                } else {
                    zox_enable(zox_id(SmoothLightsBuildSystem));
                }
            } else if (!strcmp(name->value, "Smooth Lighting")) {
                zox_smooth_lighting = value;
            }
        }
    }
} zox_sys_end(LightsSettingsDirtySystem);
