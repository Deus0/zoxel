extern entity main_app;

// NOTE: Add bone settings to our App!
/*zox_sys2(AppsSettingsSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LoadSettings);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(LoadSettings, load);
        if (load->value != zox_load_settings_spawn) {
            continue;
        }
        spawn_setting_byte(world, e, "Fullscreen", fullscreen);
        spawn_setting_byte(world, e, "VSync", vsync);
    }
} zox_sys_end(AppsSettingsSystem);*/

zox_sys2(AppsSettingsDirtySystem) {
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
                zox_log("Byte Setting [%s] Set [%i]",
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
} zox_sys_end(AppsSettingsDirtySystem);

void zox_toggle_fullscreen(ecs *world) {
    fullscreen = !fullscreen;
    if (zox_valid(main_app)) {
        zox_set_app_fullscreen(world, main_app, fullscreen);
    }
}

void zox_toggle_maximized(ecs *world) {
    maximized = !maximized;
    zox_log("TODO: Set Maximized Setting here.");
}
