// we should just add dirty hook to the settings entities

void app_initialize_lights(ecs* world, entity app) {
    spawn_setting_byte(world, app, "No Lights", zox_no_lights);
    spawn_setting_byte(world, app, "No AO", !zox_ambient_occlusion);
#ifdef zox_debug_settings
    spawn_setting_byte(world, app, "Smooth Lighting", zox_smooth_lighting);
#endif
}

void zox_events_lights(ecs* world) {
    zox_muter(prefab_app, InitializeEvent, event);
    add_to_InitializeEvent(event, app_initialize_lights);
}
