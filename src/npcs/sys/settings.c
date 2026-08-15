zox_sys2(NpcsSettingsSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LoadSettings);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(LoadSettings, load);
        if (load->value != zox_load_settings_spawn) {
            continue;
        }
        spawn_setting_byte_slider(world, e, "Npc Spawn", character_spawn_rate_max, character_spawn_rate_limits);
        spawn_setting_byte_slider(world, e, "Npc Range", character_spawn_distance, character_spawn_distance_limits);
#ifdef zox_debug_settings
        // spawn_setting_byte(world, e, "Smooth Lighting", zox_smooth_lighting);
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
            if (!strcmp(name->value, "Npc Spawn")) {
                character_spawn_rate_max = value;
            } else if (!strcmp(name->value, "Npc Range")) {
                character_spawn_distance = value;
            }
            if (dbg_log) {
                zox_log("Byte Setting [%s] Set [%i]", name->value, value);
            }
        }
    }
} zox_sys_end(NpcsSettingsDirtySystem);
