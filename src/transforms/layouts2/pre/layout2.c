// contains only pixel / transform data - invisible element
// todo: debug tools for these, using line drawing
entity spawn_prefab_layout2(ecs *world) {
    zox_prefab();
    zox_prefab_name("layout2D");

    // Local
    zox_prefab_set(e, PixelPosition, { int2_zero });
    zox_prefab_set(e, PixelSize, { int2_zero });
    zox_prefab_set(e, Anchor, { float2_zero });
    zox_prefab_set(e, LayoutPositionDirty, { zox_dirty_trigger });
    zox_prefab_set(e, LayoutSizeDirty, { zox_dirty_trigger });

    // Canvas
    zox_prefab_set(e, Layer2D, { 0 });
    zox_prefab_set(e, CanvasLink, { 0 });
    zox_prefab_set(e, CanvasPosition, { int2_zero });

    // transform2D
    zox_prefab_set(e, Position2, { float2_zero });
    zox_prefab_set(e, Rotation2D, { 0 });
    zox_prefab_set(e, Scale1D, { 1 });

    return e;
}
