// its settings, spawned on the app (InitializeEvent)
void npcs_settings(ecs* world, entity e) {
    spawn_setting_byte_slider(world, e, "Npc Spawn", character_spawn_rate_max ? character_spawn_rate_max - 1 : 0, character_spawn_rate_limits);
    spawn_setting_byte_slider(world, e, "Npc Range", character_spawn_distance, character_spawn_distance_limits);
#ifdef zox_debug_settings
    spawn_setting_byte(world, e, "No Npcs", disable_npcs);
    // spawn_setting_byte(world, e, "Smooth Lighting", zox_smooth_lighting);
#endif
}

// a setting changed (its Dirty, added again each frame it changes: a slider dragged)
void npcs_settings_on_dirty(iter* it) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ZoxName);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ZoxName, name);
        if (zox_has(e, SettingByte)) {
            byte value = zox_getv(e, SettingByte);
            if (!strcmp(name->value, "Npc Spawn")) {
                character_spawn_rate_max = value + 1;
            } else if (!strcmp(name->value, "Npc Range")) {
                character_spawn_distance = value;
            } else if (!strcmp(name->value, "No Npcs")) {
                disable_npcs = value;
            }
            if (dbg_log) {
                zox_log("Byte Setting [%s] Set [%i]", name->value, value);
            }
        }
    }
}

// spawned on the app (InitializeEvent), applied when one changes (an observer on Dirty)
void npcs_settings_events(ecs* world) {
    zox_muter(prefab_app, InitializeEvent, event);
    add_to_InitializeEvent(event, npcs_settings);
    zox_on_add(
        npcs_settings_on_dirty,
        [in] core.ZoxName,
        [none] settings.Setting,
        [none] core.Dirty,
    );
}
