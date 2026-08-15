entity spawn_prefab_layout2_root(ecs* world) {
    zox_prefab();
    zox_prefab_name("root_layout2");
    // transform2D
    zox_prefab_set(e, Position2, { float2_zero }); // float2_centre });
    // zox_prefab_set(e, Rotation2, { 0 });
    zox_prefab_set(e, Scale1, { 1 });
    // Local
    zox_prefab_set(e, LayoutSize, { int2_zero });
    zox_prefab_set(e, LayoutSizeDirty, { zox_dirty_trigger });
    return e;
}

// contains only pixel / transform data - invisible element
// todo: debug tools for these, using line drawing
entity spawn_prefab_layout2(ecs *world) {
    zox_prefab();
    zox_prefab_name("layout2");
    // transform2D
    zox_prefab_set(e, Position2, { float2_zero });
    // zox_prefab_set(e, Rotation2, { 0 });
    zox_prefab_set(e, Scale1, { 1 });
    zox_prefab_set(e, LocalPosition2, { float2_zero });
    // zox_prefab_set(e, LocalRotation2, { 0 });
    // Local
    zox_prefab_set(e, LayoutPosition, { int2_zero });
    zox_prefab_set(e, LayoutSize, { int2_zero });
    zox_prefab_set(e, LayoutPositionDirty, { zox_dirty_trigger });
    zox_prefab_set(e, LayoutSizeDirty, { zox_dirty_trigger });
    // Canvas
    zox_prefab_set(e, Anchor, { float2_zero });
    zox_prefab_set(e, Layer, { 0 });
    // zox_prefab_set(e, CanvasPosition, { int2_zero });
    // zox_prefab_set(e, CanvasLink, { 0 });
    return e;
}
