void zox_settings_apps(ecs* world, entity app) {
    spawn_setting_byte(world, app, "Fullscreen", fullscreen);
    spawn_setting_byte(world, app, "VSync", vsync);
}

void zox_settings_dirty_apps(iter* it) {
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
                zox_log("Byte Setting [apps] [%s] Set [%i]",
                    name->value,
                    value);
            }
            if (!strcmp(name->value, "Fullscreen")) {
                fullscreen = value;
                if (fullscreen_override) {
                    fullscreen = 1;
                    fullscreen_override = 0;
                }
                entity app = zox_get_parent(world, e);
                if (zox_valid(app)) {
                    zox_set_app_fullscreen(
                        world,
                        app,
                        fullscreen);
                } else {
                    zox_loge("App parent not found");
                }
            } else if (!strcmp(name->value, "VSync")) {
                vsync = value;
                on_set_vsync(vsync);
            }
        }
    }
}

