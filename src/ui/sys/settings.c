// UI Scale setting: starts as the screen's auto scale (calculate_ui_scale), a saved one overrides it
//  - applies to uis spawned after it changes (respawning the options menu would end its slider drag)
void ui_settings_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LoadSettings);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(LoadSettings, load);
        if (load->value != zox_load_settings_spawn) {
            continue;
        }
        spawn_setting_float(world, e, "UI Scale", ui_scale, (float2) { 0.5f, 3 });
    }
    zox_sys_on_end();
} zoxd_system(ui_settings_system);

void ui_settings_dirty_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SettingDirty);
    zox_sys_in(ZoxName);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SettingDirty, state);
        zox_sys_i(ZoxName, name);
        if (state->value != zox_dirty_active ||
            !zox_has(e, SettingFloat) ||
            strcmp(name->value, "UI Scale"))
        {
            continue;
        }
        const float value = zox_getv(e, SettingFloat);
        if (value > 0) {
            ui_scale = value;
        }
    }
    zox_sys_on_end();
} zoxd_system(ui_settings_dirty_system);
