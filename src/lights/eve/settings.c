void app_initialize_lights(ecs* world, entity app) {
    setting_tags(world, spawn_setting_byte(world, app, "Ambient Occlusion", zox_ambient_occlusion), zox_id(SettingTabVideo), zox_id(SettingAO));
    setting_tags(world, spawn_setting_byte(world, app, "Lighting", zox_lighting), zox_id(SettingTabVideo), zox_id(SettingLighting));
#ifdef zox_debug_settings
    setting_tags(world, spawn_setting_byte(world, app, "Smooth Lights", zox_smooth_lighting), zox_id(SettingTabVideo), zox_id(SettingSmoothLighting));
#endif
}

void lights_settings_observer_callback(iter* it) {
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
                zox_log("Setting [lights] [byte] [%s] [%i]",
                    name->value,
                    value);
            }
            if (zox_has(e, SettingLighting)) {
                zox_lighting = value;
                set_light_systems(world, zox_lighting);
            } else if (zox_has(e, SettingSmoothLighting)) {
                zox_smooth_lighting = value;
            } else if (zox_has(e, SettingAO)) {
                zox_ambient_occlusion = value;
            }
        }
    }
}

