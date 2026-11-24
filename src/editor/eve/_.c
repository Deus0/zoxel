// the "FPS" setting (Video tab): the fps display top right, in every build
void app_initialize_editor(ecs* world, entity app) {
    setting_tags(world, spawn_setting_byte(world, app, "FPS", zox_fps_display), zox_id(SettingTabVideo),
        zox_id(SettingFPSDisplay));
}

void editor_settings_observer_callback(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        if (zox_has(e, SettingFPSDisplay) && zox_has(e, SettingByte)) {
            zox_fps_display = zox_getv(e, SettingByte);
        }
    }
}

void zox_events_editor(ecs* world) {
    zox_muter(prefab_app, InitializeEvent, event);
    add_to_InitializeEvent(event, app_initialize_editor);
    zox_on_add(
        editor_settings_observer_callback,
        [in] core.ZoxName,
        [none] settings.Setting,
        [none] core.Dirty,
    );
}
