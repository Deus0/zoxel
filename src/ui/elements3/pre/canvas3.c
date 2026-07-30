entity spawn_prefab_canvas3(ecs *world) {
    zox_prefab();
    zox_prefab_name("canvas3");
    zox_add_tag(e, Canvas);
    zox_add_tag(e, Canvas3);
    zox_add_tag(e, ElementBillboard);
    // zox_prefab_set(e, Initialize, { 1 });
    zox_prefab_set(e, Position3D, { float3_zero });
    zox_prefab_set(e, Rotation3D, { float4_identity });
    zox_prefab_set(e, Scale1, { 1 });
    // zox_prefab_set(e, Scale3, { float3_one });
    zox_prefab_set(e, TransformMatrix, { float4x4_identity });
    zox_prefab_set(e, LayoutSize, { int2_zero });
    zox_prefab_set(e, LayoutSizeDirty, { 1 });
    zox_prefab_set(e, RenderDisabled, { 0 });   // used for children
    return e;
}
