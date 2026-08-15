zox_sys2(LightsSettingsSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LoadSettings);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(LoadSettings, load);
        if (load->value != zox_load_settings_spawn) {
            continue;
        }
        spawn_setting_byte(world, e, "No Lights", disable_lights);
        spawn_setting_byte(world, e, "No AO", !zox_ambient_occlusion);
#ifdef zox_debug_settings
        spawn_setting_byte(world, e, "Smooth Lighting", zox_smooth_lighting);
#endif
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
            if (!strcmp(name->value, "No Lights")) {
                disable_lights = value;
                set_light_systems(world, !disable_lights);
            } else if (!strcmp(name->value, "Smooth Lighting")) {
                zox_smooth_lighting = value;
            } else if (!strcmp(name->value, "No AO")) {
                zox_ambient_occlusion = !value;
            }
        }
    }
} zox_sys_end(LightsSettingsDirtySystem);
