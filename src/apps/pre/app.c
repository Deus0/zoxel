entity spawn_prefab_app(ecs* world, int2 position, int2 size) {
    zox_prefab();
    zox_prefab_name("app");
    zox_add(e, App);
    // this should be refactored as viewport
    zox_prefab_set(e, WindowPosition, { position });
    zox_prefab_set(e, WindowSize, { size });
    zox_prefab_set(e, WindowFullscreen, { 0 });
    zox_prefab_set(e, WindowMaximized, { 0 });
    zox_prefab_set(e, WindowSizeRestore, { int2_zero });
    zox_prefab_set(e, WindowPositionRestore, { int2_zero });
    zox_prefab_set(e, WindowMonitor, { 0 });
    zox_prefab_add(e, WindowSizeDirty);
    // Settings
    zox_prefab_set(e, ZoxName, { });
    zox_add(e, Initialize);
    zox_prefab_set(e, LoadSettings, { zox_load_settings_start });
    return e;
}

entity spawn_app(ecs* world, const char* name) {
    zox_instance(prefab_app);
    zox_name("app_sdl");
    set_ZoxName(world, e, name);
    // Link to a global
    main_app = e;
    return e;
}