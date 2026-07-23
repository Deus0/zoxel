// NOTE: Add bone settings to our App!
zox_sys2(SoundsSettingsSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LoadSettings);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(LoadSettings, load);
        if (load->value != zox_load_settings_spawn) {
            continue;
        }
        spawn_setting_float(world, e, "Volume", master_volume, (float2) { 0, 1 });
        spawn_setting_float(world, e, "Music", volume_music, (float2) { 0, 1 });
        spawn_setting_float(world, e, "SFX", volume_sfx, (float2) { 0, 1 });
    }
} zox_sys_end(SoundsSettingsSystem);

zox_sys2(SoundsSettingsDirtySystem) {
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
                zox_log("SettingFloat [%s] Set [%f]", name->value, value);
            }
            if (!strcmp(name->value, "Volume")) {
                master_volume = value;
            } else if (!strcmp(name->value, "Music")) {
                volume_music = value;
            } else if (!strcmp(name->value, "SFX")) {
                volume_sfx  = value;
            }
        }

    }
} zox_sys_end(SoundsSettingsDirtySystem);
