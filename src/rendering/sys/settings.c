float fog_settings_mul = 0.048f;

zox_sys2(RenderingSettingsSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LoadSettings);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(LoadSettings, load);
        if (load->value != zox_load_settings_spawn) {
            continue;
        }
        spawn_setting_float(world, e, "Fog", fog_density / fog_settings_mul, (float2) { 0, 1 });
        spawn_setting_byte_slider(world, e, "Downscale", viewport_downscale, (byte2) { 1, 8 });
    }
} zox_sys_end(RenderingSettingsSystem);

extern void on_set_viewport_scale(ecs*, entity);

zox_sys2(RenderingSettingsDirtySystem) {
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
        if (zox_has(e, SettingFloat)) {
            float value = zox_getv(e, SettingFloat);
            if (dbg_log) {
                zox_log("Float Setting [%s] Set [%f]", name->value, value);
            }
            if (!strcmp(name->value, "Fog")) {
                fog_density = value * fog_settings_mul;
            }
        } else if (zox_has(e, SettingByte)) {
            byte value = zox_getv(e, SettingByte);
            if (dbg_log) {
                zox_log("Byte Setting [%s] Set [%i]", name->value, value);
            }
            if (!strcmp(name->value, "Downscale")) {
                viewport_downscale = value;
                entity app = zox_get_parent(world, e);
                on_set_viewport_scale(world, app);
            }
        }
    }
} zox_sys_end(RenderingSettingsDirtySystem);
