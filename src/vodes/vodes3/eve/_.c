// we should just add dirty hook to the settings entities

void app_initialize_vodes(ecs* world, entity app) {
#ifdef zox_debug_settings
    spawn_setting_byte(world, app, "No Vodes", !zox_disable_vodes);
#endif
}

void zox_events_vodes3(ecs* world) {
    zox_muter(prefab_app, InitializeEvent, event);
    add_to_InitializeEvent(event, app_initialize_vodes);
}