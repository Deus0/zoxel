entity spawn_prefab_app(
    ecs* world,
    int2 position,
    int2 size)
{
    zox_prefab();
    zox_prefab_name("app");
    zox_add(e, App);
    // this should be refactored as viewport
    zox_setv(e, WindowPosition, position);
    zox_setv(e, WindowSize, size);
    zox_setv(e, WindowFullscreen, 0);
    zox_setv(e, WindowMaximized, 0);
    zox_setv(e, WindowSizeRestore, int2_zero);
    zox_setv(e, WindowPositionRestore, int2_zero);
    zox_setv(e, WindowMonitor, 0);
    // Settings
    zox_setv(e, ZoxName, 0);
    zox_setv(e, LoadSettings, zox_load_settings_start);
    // Events
    zox_set(e, InitializeEvent, { .count = 0 });
    zox_add(e, WindowSizeDirty);
    zox_add(e, PreInitialize);
    return e;
}

entity spawn_app(
    ecs* world,
    const char* name)
{
    zox_instance(prefab_app);
    zox_name("app_sdl");
    set_ZoxName(world, e, name);
    // Link to a global
    main_app = e;
    return e;
}
