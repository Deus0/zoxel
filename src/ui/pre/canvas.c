entity spawn_prefab_canvas(ecs *world) {

    zox_prefab();
    zox_prefab_name("canvas");
    zox_add_tag(e, Canvas);

    // Layout
    zox_prefab_set(e, LayoutPosition, { int2_zero });
    zox_prefab_set(e, LayoutSize, { int2_zero });
    zox_prefab_set(e, LayoutPositionDirty, { zox_dirty_trigger });
    zox_prefab_set(e, LayoutSizeDirty, { zox_dirty_trigger });

    // Canvas Rendering
    zox_prefab_set(e, ScreenToCanvas, { float4_zero });

    // Canvas Stack
    zox_prefab_set(e, WindowToTop, { 0 });
    zox_prefab_set(e, WindowsLayers, { 0 });
    zox_prefab_set(e, WindowsCount, { 0 });

    // Links
    zox_prefab_set(e, CameraLink, { 0 });
    zox_prefab_add(e, AppLink);
    zox_prefab_set(e, Children, { 0 });

    return e;
}
