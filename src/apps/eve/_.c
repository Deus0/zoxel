void app_initialize_apps(ecs* world, entity app) {
    spawn_setting_byte(world, app, "Fullscreen", fullscreen);
    spawn_setting_byte(world, app, "VSync", vsync);
}

void zox_apps_events(ecs* world) {
    zox_muter(prefab_app, InitializeEvent, event);
    add_to_InitializeEvent(event, app_initialize_apps);
}