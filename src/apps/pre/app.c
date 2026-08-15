entity spawn_prefab_app(ecs* world, int2 position, int2 size) {
    zox_prefab();
    zox_prefab_name("app");
    zox_add(e, App);
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
    zox_prefab_set(e, Initialize, { zox_dirty_trigger });
    zox_prefab_set(e, LoadSettings, { zox_load_settings_start });
    return e;
}
