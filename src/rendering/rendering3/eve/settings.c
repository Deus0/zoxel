void app_initialize_rendering3(ecs* world, entity app) {
    spawn_setting_byte(world, app, "Grass Animation", zox_grass_animation);
}

void rendering3_settings_observer_callback(iter* it) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ZoxName);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ZoxName, name);
        if (!zox_has(e, SettingByte)) {
            continue;
        }
        byte value = zox_getv(e, SettingByte);
        if (dbg_log) {
            zox_log("Setting [rendering3] [byte] [%s] [%i]",
                name->value,
                value);
        }
        if (!strcmp(name->value, "Grass Animation")) {
            zox_grass_animation = value;
        }
    }
}
