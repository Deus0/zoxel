void app_initialize_lights(ecs* world, entity app) {
    spawn_setting_byte(world, app, "No Lights", zox_no_lights);
    spawn_setting_byte(world, app, "No AO", !zox_ambient_occlusion);
    #ifdef zox_debug_settings
    spawn_setting_byte(world, app, "Smooth Lighting", zox_smooth_lighting);
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
            if (!strcmp(name->value, "No Lights")) {
                zox_no_lights = value;
                set_light_systems(world, !zox_no_lights);
            } else if (!strcmp(name->value, "Smooth Lighting")) {
                zox_smooth_lighting = value;
            } else if (!strcmp(name->value, "No AO")) {
                zox_ambient_occlusion = !value;
            }
        }
    }
}

