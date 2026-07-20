// NOTE: Add bone settings to our App!
zox_sys2(NpcsSettingsSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(InitializeEntity);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(InitializeEntity, state);
        if (state->value != zox_dirty_active) {
            continue;
        }
#ifdef zox_debug_settings
        spawn_setting_byte(world, e, "Disable Npcs", disable_npcs);
#endif
    }
} zox_sys_end(NpcsSettingsSystem);

zox_sys2(NpcsSettingsDirtySystem) {
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
                zox_log("SettingByte [%s] Set [%f]", name->value, value);
            }
            if (!strcmp(name->value, "Disable Npcs")) {
                disable_npcs = value;
            }
        }

    }
} zox_sys_end(NpcsSettingsDirtySystem);
