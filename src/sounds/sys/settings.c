// NOTE: Add bone settings to our App!
// its settings, spawned on the app (InitializeEvent)
void sounds_settings(ecs* world, entity e) {
    setting_tags(world, spawn_setting_float(world, e, "Volume", master_volume, (float2) { 0, 1 }), zox_id(SettingTabSound), zox_id(SettingVolume));
    setting_tags(world, spawn_setting_float(world, e, "Music", volume_music, (float2) { 0, 1 }), zox_id(SettingTabSound), zox_id(SettingMusic));
    setting_tags(world, spawn_setting_float(world, e, "SFX", volume_sfx, (float2) { 0, 1 }), zox_id(SettingTabSound), zox_id(SettingSfx));
}

// a setting changed (its Dirty, added again each frame it changes: a slider dragged)
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
            if (zox_has(e, SettingVolume)) {
                master_volume = value;
            } else if (zox_has(e, SettingMusic)) {
                volume_music = value;
            } else if (zox_has(e, SettingSfx)) {
                volume_sfx  = value;
            }
        }

    }
}

// spawned on the app (InitializeEvent), applied when one changes (an observer on Dirty)
void sounds_settings_events(ecs* world) {
    zox_muter(prefab_app, InitializeEvent, event);
    add_to_InitializeEvent(event, sounds_settings);
    zox_on_add(
        sounds_settings_on_dirty,
        [in] core.ZoxName,
        [none] settings.Setting,
        [none] core.Dirty,
    );
}
