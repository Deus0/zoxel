// NOTE: Add bone settings to our App!
// its settings, spawned on the app (InitializeEvent)
void sounds_settings(ecs* world, entity e) {
    spawn_setting_float(world, e, "Volume", master_volume, (float2) { 0, 1 });
    spawn_setting_float(world, e, "Music", volume_music, (float2) { 0, 1 });
    spawn_setting_float(world, e, "SFX", volume_sfx, (float2) { 0, 1 });
}

// a setting changed (Update, the frame after its Dirty)
void sounds_settings_on_dirty(iter* it) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ZoxName);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ZoxName, name);
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
}

// spawned on the app (InitializeEvent), applied when one changes (an observer on Update)
void sounds_settings_events(ecs* world) {
    zox_muter(prefab_app, InitializeEvent, event);
    add_to_InitializeEvent(event, sounds_settings);
    zox_on_add(
        sounds_settings_on_dirty,
        [in] core.ZoxName,
        [none] settings.Setting,
        [none] core.Update,
    );
}
