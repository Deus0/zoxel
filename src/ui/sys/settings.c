// UI Scale setting: starts as the screen's auto scale (calculate_ui_scale), a saved one overrides it
//  - applies to uis spawned after it changes (respawning the options menu would end its slider drag)
// its settings, spawned on the app (InitializeEvent)
void ui_settings(ecs* world, entity e) {
    spawn_setting_float(world, e, "UI Scale", ui_scale, (float2) { 0.5f, 3 });
}

// a setting changed (Update, the frame after its Dirty)
void ui_settings_on_dirty(iter* it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ZoxName);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ZoxName, name);
        if (!zox_has(e, SettingFloat) ||
            strcmp(name->value, "UI Scale"))
        {
            continue;
        }
        const float value = zox_getv(e, SettingFloat);
        if (value > 0) {
            ui_scale = value;
        }
    }
}

// spawned on the app (InitializeEvent), applied when one changes (an observer on Update)
void ui_settings_events(ecs* world) {
    zox_muter(prefab_app, InitializeEvent, event);
    add_to_InitializeEvent(event, ui_settings);
    zox_on_add(
        ui_settings_on_dirty,
        [in] core.ZoxName,
        [none] settings.Setting,
        [none] core.Update,
    );
}
